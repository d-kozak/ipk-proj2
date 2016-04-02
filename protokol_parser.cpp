//
// Created by david on 30.3.16.
//

#include "protokol_parser.h"

namespace requests {
	string create_success_msg() {
		return string("SUCCESS\n");
	}

	string create_eror_msg(message_id id) {
		string s = "ERROR ";
		s.append(to_string(id));
		s.append("\n");
		return s;
	}

	string create_get_file_msg(string file_name) {
		string msg = "GET FILE\n";
		msg.append("Name: " + file_name + "\n\n");
		return msg;
	}

	string create_file_transfer_msg(string file_name, size_t file_size) {
		string msg = "FILE TRANSFER\nName: ";
		msg.append(file_name);
		msg.append("\nSize: ");
		msg.append(to_string(file_size));
		msg.append("\n\n");
		return msg;
	}

	static void strip_url_to_file_name(string &file_url) {
		// check if string conctains '/'
		size_t last_slash_index = file_url.find_last_of("/");
		if (last_slash_index != string::npos) {
			file_url.erase(file_url.begin(), file_url.begin() + last_slash_index + 1);
		}
	}

	vector<char> create_file_transfer_msg_including_file_content(string file_url) {
		ifstream in_file(file_url, std::ios_base::binary);
		if (!in_file) {
			throw BaseException("File " + file_url + " was not opened successfully", ERR_FILE_NOT_OPENED);
		}
		//string req = create_file_transfer_msg_including_file_content(file_url,);

		// load content of file into vector
		std::istreambuf_iterator<char> start(in_file), end;
		vector<char> buffer(start, end);
		in_file.close();

		strip_url_to_file_name(file_url);
		string req = create_file_transfer_msg(file_url, buffer.size());


		vector<char> res(req.begin(),req.end());
		res.insert(res.end(),buffer.begin(),buffer.end());
		return res;
	}

	static void strip_response_just_to_file_name(vector<char> *response) {
		static const char *str = "Name: ";
		char *ptr = strstr(response->data(), str);
		if (ptr == NULL) {
			throw BaseException("Problem with response: parsing file name", INTERNAL_ERROR);
		}
		ptr += strlen(str);
		char *EOL = strchr(ptr, '\n');

		//response.erase(response.begin(), ptr - response.data() + response.begin());
		//response.erase(EOL - response.data() + response.begin(), response.end());

		response->clear();
		*response = vector<char>(ptr - response->data() + response->begin(),
								 EOL - response->data() + response->begin());
	}

	long remove_header_from_response(vector<char> &response, ssize_t &bytes_count) {
		static const char *str = "Size: ";
		static const char *EOH = "\n\n"; // EOH = end of head
		char *ptr = strstr(response.data(), str);
		if (ptr == NULL) {
			throw BaseException("Problem with response: parsing file name", INTERNAL_ERROR);
		}
		ptr += strlen(str);
		char *EOL = strchr(ptr, '\n');

		char *endptr;
		long size = strtol(ptr, &endptr, 10);
		if (endptr != EOL) {
			throw BaseException("Problem with response: parsing file size", INTERNAL_ERROR);
		}

		char *EOH_ptr = strstr(EOL, EOH);
		if (EOH_ptr == NULL) {
			throw BaseException("Problem with response: EOH not found", INTERNAL_ERROR);
		}
		EOH_ptr += strlen(EOH);

		bytes_count -= EOH_ptr - response.data();

		response.erase(response.begin(), EOH_ptr - response.data() + response.begin());

		return size;
	}

	message_id parse_response(vector<char> *response) {
		//vector<char> first_chars(response.begin(), response.begin() + 5);

		string first_chars(response->begin(), response->begin() + 5);
		if (first_chars.compare("SUCCE") == 0) {
			return SUCCESS;
		}
		else if (first_chars.compare("ERROR") == 0) {
			throw BaseException("ERRR Message received", stoi(response->data() + 5, NULL, 10));
		} else if (first_chars.compare("GET F") == 0) {
			strip_response_just_to_file_name(response);
			return GET_FILE;
		} else if (first_chars.compare("FILE ") == 0) {
			return FILE_TRANSFER;
		}

		string msg = "Unknown protocol message '";
		msg.append(first_chars.data());
		msg.append("'\n");
		throw BaseException(msg, ERR_INTERNAL); // unknown response
	}


	namespace client {
		void upload_requests(int socket, string file_name) {

			vector<char>req = create_file_transfer_msg_including_file_content(file_name);

			// TODO send chunks instead of one huge request
			sockets::send_message(socket, req);

			// check the response to see if the transfer was successfull
			vector<char> buffer;
			buffer.resize(sockets::HEADER_SIZE);
			sockets::read_from_socket(socket, sockets::HEADER_SIZE, buffer);

			message_id result = parse_response(&buffer);
			if (result != SUCCESS) {
				string err_msg = "ERROR ";
				err_msg.append(to_string(result));
				err_msg.append("\nThe response was invalid");
				throw BaseException(err_msg, ERR_WRONG_MSG_RECEIVED);
			}
		}

		void download_request(int socket, string file_name) {
			string req = create_get_file_msg(file_name);

			sockets::send_message(socket, req);

			vector<char> response;
			response.resize(sockets::BUFFER_SIZE);

			ssize_t size_of_transfered_data = sockets::read_from_socket(socket, sockets::HEADER_SIZE, response);
			if (parse_response(&response) != FILE_TRANSFER) {
				throw BaseException("The response was invalid", ERR_WRONG_MSG_RECEIVED);
			}

			ofstream out_file(file_name, std::ios_base::binary);
			if (!out_file) {
				throw BaseException("File " + file_name + " was not opened successfully", ERR_FILE_NOT_OPENED);
			}

			long file_size = remove_header_from_response(response, size_of_transfered_data);
			ssize_t one_transfer = size_of_transfered_data;
			while (size_of_transfered_data <= file_size) {
				response.resize(one_transfer);
				std::copy(response.begin(), response.end(), std::ostream_iterator<char>(out_file));
				response.clear();

				// when this was the last iteration, we do not want to read again
				if (size_of_transfered_data < file_size) {
					response.resize(sockets::BUFFER_SIZE);
					one_transfer = sockets::read_from_socket(socket, sockets::BUFFER_SIZE, response);
					size_of_transfered_data += one_transfer;
				}
				else
					break;
			}
			out_file.close();
		}
	}

	namespace server {
		static string get_file_name(vector<char> &buffer) {
			// TODO zjisti, proc blbne nazev pro prazdny soubor sources/empty.txt
			static const char *name = "Name: ";
			static const long __size = strlen(name);

			char *start = strstr(buffer.data(), name);
			if (start == NULL) {
				throw BaseException("Server parsing problem: name of file not found in msg", ERR_INTERNAL);
			}
			start += __size;

			char *end = strchr(start, '\n');
			if (end == NULL) {
				throw BaseException("Server parsing problem: the header seems to be deformed", ERR_INTERNAL);
			}
			vector<char> file_name(start - buffer.data() + buffer.begin(), end - buffer.data() + buffer.begin());
			return string(file_name.data());
		}

		void send_file(int socket, string file_url) {
			vector<char> message = create_file_transfer_msg_including_file_content(file_url);
			sockets::send_message(socket, message);
		}

		void store_file(int socket, vector<char> &buffer, ssize_t sum_of_transfered_data) {
			string file_name = get_file_name(buffer);

			long size = remove_header_from_response(buffer, sum_of_transfered_data);

			ofstream out_file(file_name);
			if (!out_file) {
				string exc_msg = "File ";
				exc_msg.append(file_name);
				exc_msg.append("was not opened successfully");
				throw BaseException(exc_msg, ERR_FILE_NOT_FOUND);
			}

			ssize_t one_transfer = sum_of_transfered_data;
			while (sum_of_transfered_data <= size) {
				buffer.resize(one_transfer); // resize the buffer so that it does not contain the zeros at the end
				std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(out_file));
				buffer.clear();
				// when this was the last iteration, we do not want to read again
				if (sum_of_transfered_data < size) {
					buffer.resize(sockets::BUFFER_SIZE);
					one_transfer = sockets::read_from_socket(socket, sockets::BUFFER_SIZE, buffer);
					sum_of_transfered_data += one_transfer;
				}
				else
					break;
			}
			out_file.close();

			string msg = requests::create_success_msg();
			sockets::send_message(socket, msg);
		}
	}
}