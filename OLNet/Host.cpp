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
#include <fstream>
#include <signal.h>
#include "Host.h"
#include "Packet.h"
#include "cs3516sock.h"

using namespace std;

int id = 0;
FILE* rFile;

static void terminate(int arg) {
	cout << endl << "Terminating host... " << endl;

	cout << "Closing receive file... ";
	if (rFile != NULL)
		fclose(rFile);
	cout << "Done." << endl;
	exit(1);
}

int runHost(in_addr* ip, char* file, char* destIP, char* routerIP, int ttl) {
	cout << "---------- Host Mode Started " << endl;
	cout << "File:   " << file << endl;
	cout << "HostIP: " << inet_ntoa(*ip) << endl;
	cout << "DestIP: " << destIP << endl;
	cout << "Router: " << routerIP << endl;

	signal(SIGINT, terminate);
	signal(SIGABRT, terminate);
	signal(SIGTERM, terminate);

	int sock = create_cs3516_socket();
	//cout << sock << endl;

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

		createAndSendPacket(sock, &id, (char*) &size, sizeof(uint32_t),
				inet_ntoa(*ip), destIP, routerIP, ttl, filename);
		while (sent < size) {
			int bytesIn = fread((void*) dataBuf, 1, MAX_PAYLOAD, fd);
			sent += createAndSendPacket(sock, &id, dataBuf, bytesIn,
					inet_ntoa(*ip), destIP, routerIP, ttl, filename);
			usleep(100);
		}
	} else {
		cout << "No file specified. Waiting for data." << endl;
	}

	cout << "Opening receive file... ";
	rFile = fopen("received", "w");
	if (rFile == NULL) {
		cout << "Receive file could not be opened. Exiting." << endl;
		abort();
	}
	cout << "Done." << endl;

	char* buf = new char[MAX_PACKET_SIZE];
	int recv = cs3516_recv(sock, buf, MAX_PACKET_SIZE);
	int packetCount = 0;
	while (1) {

		bzero(buf, MAX_PACKET_SIZE);
		int recv = cs3516_recv(sock, buf, MAX_PACKET_SIZE);
		packetCount++;

		cout << "Recv: " << recv << " bytes" << endl;

		packet* p = ((packet*) buf);
		char* data = (char*) (p + sizeof(packethdr));
		//char* data = (char*) (p + sizeof(packethdr) + sizeof(dataheader));

		/*if (outputToFile) {
		 errno = 0;
		 FILE* file = fopen(
		 (string("/home/cs3516/TEST") + p->datahdr.filename).c_str(),
		 "rw");

		 if (file == NULL) {
		 perror(strerror(errno));
		 exit(EXIT_FAILURE);
		 }
		 }*/

		//printPacket(p);

		cout << "Recieved " << packetCount << " packets." << endl;

		usleep(50);

		for(int i = 0; i < p->header.udp_header.len; i++){
			fputc(data[i], rFile);
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
