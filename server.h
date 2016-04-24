/*
 * IPK 2 2016
 * FTP server/client
 * author: David Kozak, xkozak15@stud.fit.vutbr.cz
 *
 * this is the main server module
 */

#ifndef IPK_PROJ2_SERVER_H
#define IPK_PROJ2_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>

#include "socket_handler.h"
#include "protokol_parser.h"

#endif //IPK_PROJ2_SERVER_H
