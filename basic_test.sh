#!/bin/bash

error() {
	echo "Error: $1" >&2
	exit 1
}

export -f error
export FILES_DIR='testfiles'
export DIR='testdir'
export CLIENT_NAME='client'
export SERVER_NAME='server'

./create_test_dirs.sh || exit 1

cd ${DIR}
./${SERVER_NAME} -p 1234 || error "Cannot start server" &


#try all clients at once
for client in $(ls client*/*); do
	
	#to send all the files
	for file in $(ls $FILES_DIR); do
		client_name=${client##*/}
		cp ${FILES_DIR}/$file ${FILES_DIR}/$client_name-${file}
		./${client} -h localhost -p 1234 -u ${FILES_DIR}/$client_name-${file} || error "Cannot start client - uploading ${file} was not successful" 
		rm ${FILES_DIR}/$client_name-${file}
	done

done

kill $(pidof ${SERVER_NAME})
