//
// Created by david on 30.3.16.
//

#ifndef IPK_PROJ2_PROTOKOL_PARSER_H
#define IPK_PROJ2_PROTOKOL_PARSER_H


#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

#include <string.h>
#include "socket_handler.h"

using namespace std;

namespace requests {
	enum message_id {
		GET_FILE,
		FILE_TRANSFER,
		SUCCESS,
		ERR_WRONG_MSG_RECEIVED,
		ERR_FILE_NOT_FOUND = 10,
		ERR_FILE_NOT_OPENED = 11,
		ERR_INTERNAL = 55
	};

	string create_success_msg();
	string create_eror_msg(message_id id);
	string create_get_file_msg(string file_name);
	string create_file_transfer_msg(string file_name, size_t file_size);
	string create_file_transfer_msg_including_file_content(string file_url);

	message_id parse_response(vector<char> *response);

	long remove_header_from_response(vector<char> &response, ssize_t &bytes_count);

	namespace client {
		void upload_requests(int socket, string file_name);
		void download_request(int socket, string file_name);
	}

	namespace server{
		void send_file(int socket,string file_url);
		void store_file(int socket,vector<char>& buffer,ssize_t sum_of_transfered_data);
	}
}

#endif //IPK_PROJ2_PROTOKOL_PARSER_H
