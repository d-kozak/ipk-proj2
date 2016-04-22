#!/usr/bin/env bash

error() {
	echo "Error: $1" >&2
	exit $2
}

export FILES_DIR='testfiles'
export DIR='testdir'
export CLIENT_NAME='client'
export SERVER_NAME='server'
export PORT_NUM=12345

./create_test_dirs.sh || exit 1

cd ${DIR}
./${SERVER_NAME} -p ${PORT_NUM} &
# todo vyresit, jak ukoncit skript, kdyz toto selze
if [ "$?" -ne 0 ]; then
	error "Server has not started successfully" 1
fi

echo "------------BASIC TEST------------"
echo ""
echo "Test of upload proccess"

for FILE in $(ls ${FILES_DIR})
do
	echo "	uploading file ${FILE}"
	#now try upload a download 
	./client1/client1 -h localhost -p ${PORT_NUM} -u ${FILES_DIR}/${FILE}

	echo "diff between original and transported file:"
	echo "------------"
	diff ${FILE} ${FILES_DIR}/${FILE}
	if [ "$?" -ne 0 ]; then
		error "File ${FILE} was changed" 1
	fi

	echo "------------"
done

echo "Test of dowload proccess"
cd client1 # go into client1 directory
for FILE in $(ls ../${FILES_DIR})
do
	echo "	downloading file ${FILE}"
	#now try upload a download 
	./client1 -h localhost -p ${PORT_NUM} -d ${FILE}

	echo "diff between original and transported file:"
	echo "------------"
	diff ./${FILE} ../${FILES_DIR}/${FILE}
	if [ "$?" -ne 0 ]; then
		error "File ${FILE} was changed" 1
	fi

	echo "------------"
done
cd ..

echo "------------END BASIC TEST------------"
echo ""
echo "------------TEST FOR WRONG ARGUMENTS, ... ------------"
cd client1
for wrong_input in ./client1 './client1 -p ${PORT_NUM}' './client1 -u' './client1 -u -rauheiurehur' './client1 -h localhost -p XXX' './client1 -h localhost -p ${PORT_NUM} -d'   './client1 -h localhost -p ${PORT_NUM} -u'
do
	${wrong_input}

	if [ "$?" -eq 0 ]; then
		error "input ${wrong_input} is wrong, program should exit" 1
	fi
done
cd ..

echo "------------END TEST FOR WRONG ARGUMENTS, ... ------------"


echo "------------STRESS TEST------------"
echo ""
#try all clients at once
for client in $(find . -type f ! -name "*.*"); do
	if [[ ${client} == './server' ]]; then
		continue
	fi
	echo ${client}
	#to send all the files
	for file in $(ls $FILES_DIR); do
		client_name=${client##*/}
		cp ${FILES_DIR}/$file ${FILES_DIR}/$client_name-${file}
		./${client} -h localhost -p ${PORT_NUM} -u ${FILES_DIR}/$client_name-${file} || error "Cannot start client - uploading ${file} was not successful" 
		rm ${FILES_DIR}/$client_name-${file}
	done

done
echo "------------END STRESS TEST------------"
echo ""

kill $(pidof ${SERVER_NAME})
