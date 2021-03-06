/*
 * IPK 2 2016
 * FTP server/client
 * author: David Kozak, xkozak15@stud.fit.vutbr.cz
 *
 * this is the main client module
 */

#include <stdexcept>
#include "client.h"

extern const bool DEBUG;

static sockets::SocketInfo parse_args(int argc, const char **argv, bool &upload, string &file_name) {
	if (argc != 7) {
		throw std::invalid_argument(
				"The number of args is incorrect, usage : client ‐h <hostname> -p <port> [‐d|u] <file_name>");
	}

	if (strcmp("-h", argv[1]) != 0) {
		throw std::invalid_argument("Firt arg must be -h");
	} else if (strcmp("-p", argv[3]) != 0) {
		throw std::invalid_argument("Third arg must be -p");
	}
	if (!(strcmp("-d", argv[5]) == 0  || strcmp("-u", argv[5]) == 0)) {
		throw std::invalid_argument("Fifth arg must be either -d or -u");
	}
	upload = strcmp("-u", argv[5]) == 0;

	file_name.clear();
	file_name.append(argv[6]);

	// todo over, zda to zda opravdu ma byt
	if(!upload && file_name.find("/") != string::npos){
		throw invalid_argument("When downloading, you cannot specify directory, the file will always be saved into the client directory");
	}

	return sockets::SocketInfo(argv[2], stoi(argv[4]));
}

static inline int send_request(const sockets::SocketInfo &socketInfo, void (*request)(int, string), string filename) {
	int socket = prepare_socket(socketInfo);

	request(socket, filename);

	return socket;
}


int main(int argc, const char *argv[]) {
	// todo soubor by mel byt ulozen do aktualniho adresare, kde je klient, momentalne se ulozi do adresare, ze ktereho je spousten
	using namespace sockets;

	try {
		bool is_upload;
		string file_name;
		SocketInfo socketInfo = parse_args(argc, argv, is_upload, file_name);

		int socket = send_request(socketInfo, is_upload ? requests::client::upload_requests : requests::client::download_request,
								  file_name);

		close_socket(socket);

	} catch (BaseException &e) {
		std::cerr << e.what();
		return e.getRetVal();
	} catch (invalid_argument &e){
		std::cerr << e.what() << endl;
		return WRONG_ARGUMENTS;
	}
	return EOK;
}
