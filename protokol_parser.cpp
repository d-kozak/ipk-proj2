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

static void strip_response_just_to_file_name(vector<char> &response){
	static const char* str = "Name: ";
	char* ptr = strstr(response.data(),str);
	if(ptr == NULL){
		throw BaseException("Problem with response: parsing file name",INTERNAL_ERROR);
	}
	ptr += strlen(str);
	char* EOL = strchr(ptr,'\n');

	response.erase(response.begin(),ptr - response.data() + response.begin());
	response.erase(EOL - response.data() + response.begin(),response.end());
}

long remove_header_from_response(vector<char>& response,ssize_t & bytes_count) {
	static const char* str = "Size: ";
	static const char* EOH = "\n\n"; // EOH = end of head
	char* ptr = strstr(response.data(),str);
	if(ptr == NULL){
		throw BaseException("Problem with response: parsing file name",INTERNAL_ERROR);
	}
	ptr += strlen(str);
	char* EOL = strchr(ptr,'\n');

	char* endptr;
	long size  = strtol(ptr,&endptr,10);
	if(ptr != EOL){
		throw BaseException("Problem with response: parsing file size",INTERNAL_ERROR);
	}

	char* EOH_ptr = strstr(EOL,EOH);
	if(EOH_ptr == NULL){
		throw BaseException("Problem with response: EOH not found",INTERNAL_ERROR);
	}

	bytes_count -= EOH_ptr - response.data();

	return size;
}

message_id parse_response(vector<char> &response) {
	vector<char> first_chars(response.begin(),response.begin() + 5);


	if(!strcmp(first_chars.data(),"SUCCE")){
		return SUCCESS;
	}
	else if (!strcmp(first_chars.data(), "ERROR")) {
		throw BaseException("ERRR Message received",stoi(first_chars.data(), NULL, 10));
	} else if (!strcmp(first_chars.data(),"GET F")) {
		strip_response_just_to_file_name(response);
		return GET_FILE;
	} else if (!(first_chars.data(),"FILE ")) {
		// TODO toto spise provest az v ramci dalsiho zpracovani: remove_header_from_response(response);
		return FILE_TRANSFER;
	}
	throw BaseException("Unknown protocol message",ERR_INTERNAL); // unknown respose
}