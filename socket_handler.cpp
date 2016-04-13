//
// Created by david on 10.3.16.
//

#include "socket_handler.h"

namespace sockets {

	int prepare_socket(const SocketInfo &socket_info) {
		int client_socket;

		addrinfo hint;
		addrinfo *server_info;

		memset(&hint, '\0', sizeof(addrinfo));
		hint.ai_family = AF_UNSPEC;
		hint.ai_socktype = SOCK_STREAM;
		hint.ai_protocol = IPPROTO_TCP;

		int ret = getaddrinfo(socket_info.getHostname().c_str(), to_string(socket_info.getPort()).c_str(), &hint,
							  &server_info);
		if (ret != 0) {
			throw BaseException("Invalid hostname", GET_HOST_BY_NAME_ERROR);
		}

		addrinfo *tmp;
		for (tmp = server_info; tmp != NULL;
			 tmp = tmp->ai_next) {
			if((client_socket = socket(tmp->ai_family,tmp->ai_socktype,tmp->ai_protocol)) == -1){
				continue;
			}
			if(connect(client_socket,tmp->ai_addr,tmp->ai_addrlen) == -1)
				continue;
			break;
		}

		if(tmp == NULL)
			throw  BaseException("Connection was not established successfully",CONNECT_ERROR);

		return client_socket;
	}

	void send_message(int socket, string message) {

		ssize_t bytes_count = 0;
		//std::cout << "size: " << msg.size() << "strlen: " << strlen((msg.c_str())) << "\n";
		bytes_count = send(socket, message.c_str(), message.size(), 0);
		if (bytes_count < 0) {
			perror("ERROR: sendto");
			throw BaseException("Request was not send successfully\n", SEND_ERROR);
		}
	}

	void send_message(int socket, vector<char> message) {

		ssize_t bytes_count = 0;
		//std::cout << "size: " << msg.size() << "strlen: " << strlen((msg.c_str())) << "\n";
		bytes_count = send(socket, message.data(), message.size(), 0);
		if (bytes_count < 0) {
			perror("ERROR: sendto");
			throw BaseException("Request was not send successfully\n", SEND_ERROR);
		}
	}

	void close_socket(int fd) {
		if (close(fd) != 0) {
			perror("ERROR: close");
			throw BaseException("Closing of socket was not successfull", CLOSE_ERROR);
		}
	}

	/**
	 * read data from socket
	 * @param (int) socket socket descriptor
	 * @param (unsigned long) size of the buffer
	 * @param (vector<char> &) buffer
	 * @return (ssize_t) number of bytes loaded
	 */
	ssize_t read_from_socket(int socket, unsigned long size, vector<char> &buffer) {
		buffer.resize(size);

		ssize_t bytes_count = recv(socket, buffer.data(), size, 0);

		if (bytes_count < 0) {
			perror("ERROR: recvfrom");
			throw BaseException("The transminsion of buffer was not successfull", RECV_ERROR);
		}
		return bytes_count;
	}

	namespace server {
		int prepare_socket_and_int_vars(int port_number, sockaddr_in6 &sa, int &rc) {
			int welcome_socket;
			if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
				perror("ERROR: socket");
				exit(EXIT_FAILURE);
			}

			memset(&sa, 0, sizeof(sa));
			sa.sin6_family = AF_INET6;
			sa.sin6_addr = in6addr_any;
			sa.sin6_port = htons(port_number);


			if ((rc = bind(welcome_socket, (struct sockaddr *) &sa, sizeof(sa))) < 0) {
				perror("ERROR: bind");
				exit(EXIT_FAILURE);
			}
			if ((listen(welcome_socket, 1)) < 0) {
				perror("ERROR: listen");
				exit(EXIT_FAILURE);
			}
			return welcome_socket;
		}
	}
}

/**
 * Function prints the reponse without chunks into file output_file
 */
/*
static void print_without_chunk_numbers(vector<char> &data, ofstream &output_file) {
	long i = 0, end = (int) data.size(), chunk_size;
	vector<char> chunk_num;

	size_t next_char;
	// loop through the whole data vector
	while (i < end) {
		// parse the chunk size(it always ends with \r\n
		while (data[i] != '\r') {
			chunk_num.push_back(data[i]);
			i++;
		}
		i += 2; //skip the "\r\n"

		if (chunk_num.size() == 1 && chunk_num[0] == '0') // no more data to print
			break;

		// get the size of the current chunk
		chunk_size = stol(chunk_num.data(), &next_char, 16);

		std::copy(data.begin() + i, data.begin() + i + chunk_size, std::ostream_iterator<char>(output_file));

		//jump to next chunk
		i += chunk_size + 2;
		// clear the vector for storing chunk_size;
		chunk_num.clear();
	}
}
 */



