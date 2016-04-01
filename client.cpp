/*
 * IPK.2015L
 *
 * Demonstration of trivial TCP communication.
 *
 * Ondrej Rysavy (rysavy@fit.vutbr.cz)
 *
 */
#include <stdexcept>
#include "client.h"


static sockets::SocketInfo parse_args(int argc, const char **argv, bool &upload, string &file_name) {
	if (argc != 7) {
		throw std::invalid_argument(
				"The number of args is incorrect, usage : client ‐h <hostname> -p <port> [‐d|u] <file_name>");
	}

	if (!strcmp("-h", argv[1])) {
		throw std::invalid_argument("Firt arg must be -h");
	} else if (!strcmp("-p", argv[3])) {
		throw std::invalid_argument("Third arg must be -p");
	}
	if (!strcmp("-d", argv[5]) || !strcmp("-u", argv[5])) {
		throw std::invalid_argument("Fifth arg must be either -d or -u");
	}
	upload = strcmp("-u", argv[5]) == 0;

	file_name.clear();
	file_name.append(argv[6]);

	return sockets::SocketInfo(argv[2], stoi(argv[4]));
}

static void upload_request(int socket, string file_name) {
	ifstream in_file(file_name,std::ios_base::binary);
	if(in_file.bad()){
		throw BaseException("File " + file_name +  " was not opened successfully",ERR_FILE_NOT_OPENED);
	}
	//string req = create_file_transfer_msg(file_name,);
	// TODO load real data to to vector
	vector<char> buffer;
	buffer.push_back('x');

	string req = create_file_transfer_msg(file_name, buffer.size());

	// TODO send chunks instead of one huge request
	sockets::send_message(socket,req);

	buffer.clear();

	ssize_t bytes_count = sockets::read_from_socket(socket, sockets::HEADER_SIZE, buffer);

	message_id result = parse_response(buffer);
	if(result != SUCCESS){
		throw BaseException("The response was invalid",ERR_WRONG_MSG_RECEIVED);
	}
}

static void download_request(int socket, string file_name) {
	ofstream out_file(file_name,std::ios_base::binary);
	if(out_file.bad()){
		throw BaseException("File " + file_name +  " was not opened successfully",ERR_FILE_NOT_OPENED);
	}

	string req = create_get_file_msg(file_name);

	sockets::send_message(socket,req);

	vector<char> response;

	ssize_t bytes_count = sockets::read_from_socket(socket,sockets::HEADER_SIZE,response);
	if(parse_response(response) != FILE_TRANSFER){
		throw BaseException("The response was invalid",ERR_WRONG_MSG_RECEIVED);
	}

	long file_size = remove_header_from_response(response,bytes_count);

	while(bytes_count < file_size){
		std::copy(response.begin(),response.end(),out_file);
		response.clear();

		bytes_count += sockets::read_from_socket(socket,sockets::BUFFER_SIZE,response);
	}


}

static inline int send_request(const sockets::SocketInfo &socketInfo, void (*request)(int, string), string filename) {
	int socket = prepare_socket(socketInfo);

	request(socket, filename);

	return socket;
}


int main(int argc, const char *argv[]) {

	using namespace sockets;

	try {
		bool is_upload;
		string file_name;
		SocketInfo socketInfo = parse_args(argc, argv, is_upload, file_name);

		int socket = send_request(socketInfo, is_upload ? upload_request : download_request, file_name);


		vector<char> response;

		read_from_socket(socket, sockets::HEADER_SIZE, response);

		cout << response.data();

		close_socket(socket);

	} catch (BaseException &e) {
		std::cerr << e.what();
		return e.getRetVal();
	} catch (invalid_argument &e){
		std::cerr << e.what();
		return WRONG_ARGUMENTS;
	}
	return EOK;
}
