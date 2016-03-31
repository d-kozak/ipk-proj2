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

SERVER_FILES=server.o protokol_parser.o
CLIENT_FILES=client.o protokol_parser.o

#seznamy vsech .cpp,.h a .o souboru
OBJ=server.o client.o protokol_parser.o
HEAD=server.h client.h protokol_parser.h
SRC=server.cpp client.cpp protokol_parser.cpp

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
	tar -cvzf xkozak15.tar.gz ${HEAD} ${SRC} Makefile

clean:
	rm -f *.o ${program}
	rm -f xkozak15.zip