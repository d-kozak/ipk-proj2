//
// Created by david on 30.3.16.
//

#include "protokol_parser.h"

string create_eror_msg(message_id id) {
	return "ERROR " + id + '\n';
}

string create_get_file_msg(string file_name) {
	string msg = "GET FILE\n";
	msg.append("Name: " + file_name + "\n\n");
	return msg;
}

string create_file_transfer_msg(string file_name, size_t file_size) {
	string msg = "FILE TRANSFER\n";
	msg.append("Name: " + file_name + "\n");
	msg.append("Size: " + file_size + '\n' + '\n');
	return msg;
}

static void get_file_name_from_response(vector<char>& response){
	// TODO implement
}

static void remove_header_from_response(vector<char>& response){
	// TODO implement
}

message_id parse_response(vector<char> &response, string &return_val) {
	return_val.clear();
	vector<char> first_chars(response.begin(),response.begin() + 5);


	if (!strcmp(first_chars.data(), "ERROR")) {
		return static_cast<message_id >(stoi(first_chars.data(), NULL, 10));
	} else if (!strcmp(first_chars.data(),"GET F")) {
		get_file_name_from_response(response);
		return GET_FILE;
	} else if (!(first_chars.data(),"FILE ")) {
		remove_header_from_response(response);
		return FILE_TRANSFER;
	}
	throw BaseException("Unknown protocol message",ERR_INTERNAL); // unknown respose
}