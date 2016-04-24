/*
 * IPK 2 2016
 * FTP server/client
 * author: David Kozak, xkozak15@stud.fit.vutbr.cz
 *
 * this is the main server module
 */

#include <sys/wait.h>
#include <map>
#include <mutex>
#include "server.h"

extern const bool DEBUG;

volatile sig_atomic_t flag = 0;

/**
 * function handles catching SIGCHILD signal, nothing really needs to be done here now
 */
void SigCatcher(int n) {
	int pid = wait3(NULL, WNOHANG, NULL);
	if (DEBUG)
		printf("Child %d spawned.\n", pid);
}

/**
 * function handles catching SIGINT signal, releases allocate locks and kills the proccess
 */
void SigIntCatch(int n){
	std::cerr << "Received sigint, I will set the flag" << std::endl;
	flag = 1;
	requests::server::release_locks();
	exit(1);
}


/**
 * parse the arguments
 * @return port number
 */
static int parse_args(int argc, const char **argv) {
	if (argc != 3) {
		throw std::invalid_argument("Wrong number of arguments, usage: server -p <port_number>");
	}
	if (strcmp("-p", argv[1]) != 0) {
		throw std::invalid_argument("First argument must be -p");
	}
	return std::stoi(argv[2]);
}


/**
 * handles once connestion
 */
static void handle_connection(int comm_socket) {
	vector<char> buffer;
	buffer.resize(sockets::HEADER_SIZE);
	// read first part of msg
	ssize_t bytes_count = sockets::read_from_socket(comm_socket, sockets::HEADER_SIZE, buffer);

	// then decide what to do...
	try {
		switch (requests::parse_response(buffer)) {
			case requests::GET_FILE:
				requests::server::send_file(comm_socket, buffer.data());
				break;
			case requests::FILE_TRANSFER:
				requests::server::store_file(comm_socket, buffer, bytes_count);
				break;
			default:
				throw BaseException("Default in switch in handle connection", INTERNAL_ERROR);
		}
	} catch (BaseException &e) {
		// if there was a problem, simply send error msg back
		cerr << "It appears there was a problem with this request..." << endl;
		cerr << e.what();
		string msg = requests::create_eror_msg(static_cast<requests::message_id>(e.getRetVal()));
		cerr << msg;
		sockets::send_message(comm_socket, msg);
	}
}

/*
 * forks the server , child will handle the connection
 */
static void lets_do_the_forknam_style(int comm_socket, int welcome_socket) {
	int pid = fork();
	if (pid < 0) {
		perror("fork failed");
		throw BaseException("Forking failed", FORK_ERROR);
	}
	if (pid == 0) {
		// child process
		close(welcome_socket);
		handle_connection(comm_socket);
		exit(0); // kill the process
	} else {
		close(comm_socket);
	}

}

/**
 * for debugging purposes only
 */
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

		signal(SIGCHLD, SigCatcher);
		signal(SIGINT, SigIntCatch);

		// main server loop - endless
		while (1) {
			if(flag){
				std::cerr << "Termination flag is on, I will release locks and  shut myself down" << std::endl;
				break;
			}

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
			if(DEBUG)
				printf("Connection to %s closed\n", str);
			close(comm_socket);
		}
	} catch (BaseException &e) {
		cerr << "INFO: TOP LEVEL catch in main\n";
		cerr << e.what();
		requests::server::release_locks();
		return e.getRetVal();
	} catch (std::invalid_argument &e) {
		cerr << "INFO: TOP LEVEL catch in main\n";
		cerr << e.what() << endl;
		requests::server::release_locks();
		return WRONG_ARGUMENTS;
	}

	requests::server::release_locks();
	return 0;
}
