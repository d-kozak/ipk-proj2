//
// Created by david on 1.4.16.
//

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

static const int BUFFER_SIZE = 2048;
static const int HEADER_SIZE = 320;

using namespace std;

enum ret_val {
	EOK,
	WRONG_ARGUMENTS,
	URL_PARSE_ERROR,
	SOCKET_ERROR,
	GET_HOST_BY_NAME_ERROR,
	CONNECT_ERROR,
	SEND_ERROR,
	RECV_ERROR,
	CLOSE_ERROR,
	INTERNAL_ERROR
};


class Socket_info {
	string hostname;
	int port;

public:
	Socket_info(const string &hostname, int port) : hostname(hostname), port(port) { }


	const string &getHostname() const {
		return hostname;
	}

	int getPort() const {
		return port;
	}
};


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


#endif //IPK_PROJ2_SOCKET_HANDLER_H
