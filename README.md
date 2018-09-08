# IPK projekt 2 : Program for file transfer
## Autor: David Kozák, xkozak15
## Kontakt: xkozak15@stud.fit.vutbr.cz

Client server app written in C++ that allows file transfer. It uses a custom protocol that was designed as part of the assignment.

Compilie the project using ```make```
Clean the project using ```make clean```


## Files

* ./protokol.pdf - description of the communication protocol

* ./protokol_parser - module for creating and parsing messages
* ./socket_handler - module working with sockets, sending and receiving messages

* ./server - module implementing the  server
* ./client - module implementing the client

* ./test.sh - test file demonstrating the funcionality

* ./create_test_dirs.sh - helper script which creates the structure for testing
	which looks as following
		./testdir - root directory
		./testdir/server - executable file starting the server
		./testdir/client{1-9} - directories for individual clients
		./testdir/testfiles - directory with files used in testing

* ./testfiles/* - files used in testing
* ./Makefile 

## Usage
```nohighlight
./client -h hostname -p port [-d|-u] file_name
	hostname - name/ip address of server
	port - port number
	-d download, -u upload
	file_name - file to transfer

./server -p port
	port - port number on which the server should listen

```
