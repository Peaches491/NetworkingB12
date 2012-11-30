/*
 * Host.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "Host.h"
#include "Packet.h"
#include "cs3516sock.h"

using namespace std;

int runHost(int argc, char* argv[]) {
	cout << "---------- Host Mode Started" << endl;

	int sock = create_cs3516_socket();

	if(argc > 1){
		FILE* fd = fopen(argv[2], "r");
		char * dataBuf = new char[MAX_PAYLOAD];

		int size = getFileSize(fd);
		int sent = 0;

		while (sent < size){
			int bytesIn = fread((void*)dataBuf, 1, MAX_PAYLOAD, fd);
			sent += createAndSendPacket(sock, dataBuf, bytesIn, argv[3]);
		}
	}

	char* buf = new char[MAX_PACKET_SIZE];

	while (1) {
		bzero(buf, MAX_PACKET_SIZE);
		int recv = cs3516_recv(sock, buf, 1000);

		packet* p = ((packet*) buf);

		cout << endl;
		cout << "Recv: " << recv << " bytes" << endl;

		cout << "Header Size: " << sizeof(packethdr) << " bytes" << endl;
		cout << "Data Size: " << p->header.udp_header.len << " bytes"
				<< endl;

		printPacket(p);

		//char* data = buf + sizeof(packethdr);
		//cout << data << endl;
	}
	return 0;
}

int getFileSize(FILE* fd){
	int size = -1;
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	return size;
}
