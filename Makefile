#Makefile pro projekt 2 do IPK  - ftp client/server

#prekladac c
CC=clang++

#mira optimalizace
OPT=-O2

#zakladni parametry prekladu
CPPFLAGS= -std=c++11 -Wall -Wextra -pedantic -g

#nazev programu
CLIENT_NAME=client
SERVER_NAME=server

SERVER_FILES=server.o protokol_parser.o socket_handler.o
CLIENT_FILES=client.o protokol_parser.o socket_handler.o

#seznamy vsech .cpp,.h a .o souboru
OBJ=server.o client.o protokol_parser.o socket_handler.o
HEAD=server.h client.h protokol_parser.h socket_handler.h
SRC=server.cpp client.cpp protokol_parser.cpp socket_handler.cpp

#soubory pro testovani
TEST_FILES=test.sh create_test_dirs.sh testfiles/*

.PHONY: build
.PHONY: clean
.PHONY: distrib

build: ${SERVER_NAME} ${CLIENT_NAME}

${SERVER_NAME}: ${SERVER_FILES}
	${CC} ${SERVER_FILES} -o ${SERVER_NAME} ${OPT} -g

${CLIENT_NAME}: ${CLIENT_FILES}
	${CC} ${CLIENT_FILES} -o ${CLIENT_NAME} ${OPT} -g

#zavislost vsech .o na .h
${OBJ}: ${HEAD}

distrib:
	tar -cvzf xkozak15.tar.gz ${HEAD} ${SRC} ${TEST_FILES} Makefile 

clean:
	rm -f *.o ${CLIENT_NAME} ${SERVER_NAME}
	rm -f xkozak15.tar.gz
