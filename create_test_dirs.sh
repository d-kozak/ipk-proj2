#!/bin/bash

export -f error
export FILES_DIR='testfiles'
export DIR='testdir'
export CLIENT_NAME='client'
export SERVER_NAME='server'

make clean

make || error "Build was not successfull" # build the binaries

if [[ -d  ${DIR} ]]; then
		rm -rf ${DIR}
fi

mkdir ${DIR}
cp ${SERVER_NAME} ${DIR}/${SERVER_NAME}

for i in 1 2 3 4 5 6 7 8 9 ; do 
	mkdir ${DIR}/${CLIENT_NAME}${i}
	cp ${CLIENT_NAME} ${DIR}/${CLIENT_NAME}${i}/${CLIENT_NAME}${i}
done

cp -r ${FILES_DIR} ${DIR}/

exit 0

