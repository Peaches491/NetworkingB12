/*
 * Host.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <cerrno>
#include "Host.h"
#include "Packet.h"
#include "cs3516sock.h"

using namespace std;

int runHost(char* file, char* destIP, char* routerIP) {
	cout << "---------- Host Mode Started " << endl;
	cout << "File:   " << file << endl;
	cout << "DestIP: " << destIP << endl;
	cout << "Router: " << routerIP << endl;

	int sock = create_cs3516_socket();
	cout << sock << endl;

	if (strcmp(file, "") != 0) {
		string fileString = string(file);
		cout << "Got a file!" << endl;
		FILE* fd = fopen(file, "r");

		std::string filename;

		size_t pos = fileString.find_last_of("/");
		if (pos != std::string::npos)
			filename.assign(fileString.begin() + pos + 1, fileString.end());
		else
			filename = fileString;

		char * dataBuf = new char[MAX_PAYLOAD];

		int size = getFileSize(fd);
		int sent = 0;

		while (sent < size) {
			int bytesIn = fread((void*) dataBuf, 1, MAX_PAYLOAD, fd);
			sent += createAndSendPacket(sock, dataBuf, bytesIn, destIP,
					routerIP, filename);
		}
	} else {
		cout << "No file specified. Waiting for data." << endl;
	}

	char* buf = new char[MAX_PACKET_SIZE];

	while (1) {

		bool outputToFile = false;

		bzero(buf, MAX_PACKET_SIZE);
		int recv = cs3516_recv(sock, buf, MAX_PACKET_SIZE);

		packet* p = ((packet*) buf);
		char* data = (char*) (p + sizeof(packethdr));
		//char* data = (char*) (p + sizeof(packethdr) + sizeof(dataheader));

		FILE* file;
//		if (outputToFile) {
//			errno = 0;
//			FILE* file = fopen(
//					(string("/home/cs3516/TEST") + p->datahdr.filename).c_str(),
//					"rw");
//
//			if (file == NULL) {
//				perror(strerror(errno));
//				exit(EXIT_FAILURE);
//			}
//		}

		cout << endl;
		cout << "Recv: " << recv << " bytes" << endl;

		cout << "Header Size: " << sizeof(packethdr) << " bytes" << endl;
		cout << "Data Size: " << p->header.udp_header.len << " bytes" << endl;

		printPacket(p);

		if (outputToFile) {
			fprintf(file, data);
		}

		//char* data = buf + sizeof(packethdr);
		//cout << data << endl;
	}
	return 0;
}

int getFileSize(FILE* fd) {
	int size = -1;
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	return size;
}
