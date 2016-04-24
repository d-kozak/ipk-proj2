/*
 * IPK 2 2016
 * FTP server/client
 * author: David Kozak, xkozak15@stud.fit.vutbr.cz
 *
 * this is module handles communication via TCP BSD sockets
 */
#ifndef IPK_PROJ2_SOCKET_HANDLER_H
#define IPK_PROJ2_SOCKET_HANDLER_H

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <iterator>
#include <netinet/in.h>
#include <vector>

/* true = print debug info */
const bool DEBUG = false;

using namespace std;

/**
 * return values
 */
enum ret_val {
	EOK,
	WRONG_ARGUMENTS,
	SOCKET_ERROR,
	GET_HOST_BY_NAME_ERROR,
	CONNECT_ERROR,
	SEND_ERROR,
	RECV_ERROR,
	CLOSE_ERROR,
	FORK_ERROR,
	INTERNAL_ERROR
};

/**
 * the one and only exception to handle various errors
 */
class BaseException {
	std::string msg;
	int ret_val;

public:
	BaseException(std::string msg, int ret_val) : ret_val(ret_val) {
		this->msg = "ERROR: ";
		this->msg.append(msg + "\n");
	}

	const std::string &what() {
		return this->msg;
	}

	int getRetVal() {
		return this->ret_val;
	}
};

namespace sockets {
	/**size of one msg read from socket**/
	const int BUFFER_SIZE = 2048;
	/** the first msg can be shorter**/
	const int HEADER_SIZE = 320;

	/**
	 * info about socket - hostname and port, used in client
	 */
	class SocketInfo {
		string hostname;
		int port;

	public:
		SocketInfo(const string &hostname, int port) : hostname(hostname), port(port) { }


		const string &getHostname() const {
			return hostname;
		}

		int getPort() const {
			return port;
		}
	};


	/**
	 * prepare tcp socket
	 * @param (SocketInfo&) socket_info
	 */
	int prepare_socket(const SocketInfo &socket_info);

	/**
	 *
	 * @param (int) socket descriptor
	 * @param (string) message
	 */
	void send_message(int socket, string message);
	void send_message(int socket, vector<char> message);

	/**
	 * @param (int) socket descriptor
	 */
	void close_socket(int socket);


	/**
	 * read data from socket
	 * @param (int) socket socket descriptor
	 * @param (unsigned long) size of the buffer
	 * @param (vector<char> &) buffer
	 * @return (ssize_t) number of bytes loaded
	 */
	ssize_t read_from_socket(int socket, unsigned long size, vector<char>& buffer);

	namespace server{
		/**
		 * prepares socket for server
		 */
		int prepare_socket_and_int_vars(int port_number, sockaddr_in6 &sa, int &rc);
	}
}


#endif //IPK_PROJ2_SOCKET_HANDLER_H
