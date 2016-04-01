//
// Created by david on 10.3.16.
//

#include "socket_handler.h"

namespace sockets {

	int prepare_socket(const SocketInfo &socket_info) {
		int client_socket;
		if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
			perror("ERROR: socket");
			throw BaseException("Socket was not created succesfully", SOCKET_ERROR);
		}

		hostent *server = gethostbyname(socket_info.getHostname().c_str());
		if (server == NULL) {
			fprintf(stderr, "ERROR, no such host  as %s.\n", socket_info.getHostname().c_str());
			throw BaseException("DNS translation was not succesfull", GET_HOST_BY_NAME_ERROR);
		}

		struct sockaddr_in server_address;
		bzero((char *) &server_address, sizeof(server_address));
		server_address.sin_family = AF_INET;
		bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, (size_t) server->h_length);

		server_address.sin_port = htons((uint16_t) socket_info.getPort());

		if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
			perror("ERROR: connect");
			throw BaseException("Connection was not started successfully\n", CONNECT_ERROR);
		}
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
	ssize_t read_from_socket(int socket, unsigned long size,vector<char>& buffer) {
		buffer.resize(size);

		ssize_t bytes_count = recv(socket, buffer.data(), size, 0);

		if (bytes_count < 0) {
			perror("ERROR: recvfrom");
			throw BaseException("The transminsion of buffer was not successfull", RECV_ERROR);
		}
		return bytes_count;
	}


	namespace server{
		// TODO add functions needed by server
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



