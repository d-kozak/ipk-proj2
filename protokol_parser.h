/*
 * IPK 2 2016
 * FTP server/client
 * author: David Kozak, xkozak15@stud.fit.vutbr.cz
 *
 * this is module handles creating and parsing of protocol requests
 * @see protokol.pdf
 */

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

	/**
	 * creates msg representing success, whatever that means in given context
	 */
	string create_success_msg();
	/**
	 * creates msg representing error specified by
	 * @param id - err type
	 */
	string create_eror_msg(message_id id);

	/**
	 * creates request for for file with given name
	 */
	string create_get_file_msg(string file_name);

	/**
	 * created msg representing given file
	 */
	string create_file_transfer_msg(string file_name, size_t file_size);
	vector<char> create_file_transfer_msg_including_file_content(string file_url);

	/**
	 * parse response from client/server
	 * @return type of msg
	 */
	message_id parse_response(vector<char> &response);

	/**
	 * removes header, leaving only file data
	 */
	long remove_header_from_response(vector<char> &response, ssize_t &bytes_count);

	/**
	 * strips whole url just to file name
	 */
	void strip_url_to_file_name(string &file_url);

	/**
	 * functions specific just for client
	 */
	namespace client {
		/**
		 * creates and sends upload request
		 */
		void upload_requests(int socket, string file_name);
		/**
		 * creates and sends download request
		 */
		void download_request(int socket, string file_name);
	}

	/**
	 * functions specific just for server
	 */
	namespace server{
		/**
		 * load content of given file and sent it to client
		 */
		void send_file(int socket,string file_url);
		/**
		 * store file given in buffer
		 */
		void store_file(int socket,vector<char>& buffer,ssize_t sum_of_transfered_data);


		/**
		 * internal clean-up - each file has its own lock - for synchronization purpossess
		 */
		void release_locks();
	}
}

#endif //IPK_PROJ2_PROTOKOL_PARSER_H
