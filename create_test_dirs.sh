make clean

make || exit 55 # build the binaries

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

