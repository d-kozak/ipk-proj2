/*
 * IPK.2015L
 *
 * Demonstration of trivial TCP communication.
 *
 * Ondrej Rysavy (rysavy@fit.vutbr.cz)
 *
 */
#include "client.h"

void parse_args(int argc, const char **argv) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return sockets::SocketInfo(argv[1], stoi(argv[2]));
}


int main(int argc, const char *argv[]) {

	using namespace sockets;

	try {
		SocketInfo socketInfo = parse_args(argc, argv);

		int socket = prepare_socket(socketInfo);

		send_message(socket, "Zprava 1");

		vector<char> response;

		read_from_socket(socket, sockets::HEADER_SIZE, response);

		cout << response.data();

		close_socket(socket);

	} catch (BaseException &e) {
		std::cerr << e.what();
		return e.getRetVal();
	}
}
