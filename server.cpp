/*
 * IPK.2015L
 *
 * Demonstration of trivial TCP communication.
 *
 * Ondrej Rysavy (rysavy@fit.vutbr.cz)
 *
 */

#include <sys/wait.h>
#include "server.h"

extern const bool DEBUG;

void SigCatcher(int n)
{
	int pid = wait3(NULL,WNOHANG,NULL);
	printf("Child %d spawned.\n",pid);
}

static int parse_args(int argc, const char **argv) {
	if (argc != 3) {
		throw std::invalid_argument("Wrong number of arguments, usage: server -p <port_number>");
	}
	if (strcmp("-p", argv[1]) != 0) {
		throw std::invalid_argument("First argument must be -p");
	}
	return std::stoi(argv[2]);
}

static void handle_connection(int comm_socket) {
	vector<char> buffer;
	buffer.resize(sockets::HEADER_SIZE);
	ssize_t bytes_count = sockets::read_from_socket(comm_socket, sockets::HEADER_SIZE, buffer);

	try {
		switch (requests::parse_response(&buffer)) {
			case requests::GET_FILE:
				requests::server::send_file(comm_socket, buffer.data());
				break;
			case requests::FILE_TRANSFER:
				requests::server::store_file(comm_socket, buffer,bytes_count);
				break;
			default:
				throw BaseException("Default in switch in handle connection", INTERNAL_ERROR);
		}
	} catch (BaseException &e) {
		cerr << "It appears there was a problem with this request..." << endl;
		cerr << e.what();
		string msg = requests::create_eror_msg(static_cast<requests::message_id>(e.getRetVal()));
		cerr << msg;
		sockets::send_message(comm_socket,msg);
	}
}

static void lets_do_the_forknam_style(int comm_socket, int welcome_socket){
	int pid = fork();
	if(pid < 0){
		perror("fork failed");
		throw BaseException("Forking failed",FORK_ERROR);
	}
	if(pid == 0){
		// child process
		close(welcome_socket);
		handle_connection(comm_socket);
		exit(0); // kill the process
	} else {
		close(comm_socket);
	}

}

void log_connection(sockaddr_in6 *sa_client, char (&str)[INET6_ADDRSTRLEN]) {
	if (inet_ntop(AF_INET6, &(sa_client->sin6_addr), str, sizeof(str))) {
		if (DEBUG) {
			printf("INFO: New connection:\n");
			printf("INFO: Client address is %s\n", str);
			printf("INFO: Client port is %d\n", ntohs(sa_client->sin6_port));
		}
	}
}

int main(int argc, const char *argv[]) {
	int rc;
	struct sockaddr_in6 sa;
	struct sockaddr_in6 sa_client;
	char str[INET6_ADDRSTRLEN];
	socklen_t sa_client_len = sizeof(sa_client);

	try {

		int port_number = parse_args(argc, argv);

		int welcome_socket = sockets::server::prepare_socket_and_int_vars(port_number, sa, rc);

		signal(SIGCHLD,SigCatcher);

		// disable the endless loop warning
		while (1) {
			int comm_socket = accept(welcome_socket, (struct sockaddr *) &sa_client, &sa_client_len);
			if (comm_socket > 0) {
				log_connection(&sa_client, str);
				lets_do_the_forknam_style(comm_socket, welcome_socket);
			}
			else {
				//throw BaseException("Connection was not created successfully",ERR_INTERNAL);
				puts(".");
				cerr << "ERRROR: connection was not created successfully";
			}
			printf("Connection to %s closed\n", str);
			close(comm_socket);
		}
	} catch (BaseException &e) {
		cerr << "INFO: TOP LEVEL catch in main\n";
		cerr << e.what();
		return e.getRetVal();
	} catch (std::invalid_argument &e) {
		cerr << "INFO: TOP LEVEL catch in main\n";
		cerr << e.what() << endl;
		return WRONG_ARGUMENTS;
	}
}
