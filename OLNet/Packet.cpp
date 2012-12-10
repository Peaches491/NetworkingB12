/*
 * OLPacket.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cs3516sock.h"
#include "Packet.h"

using namespace std;

int createAndSendPacket(int sock, int* id, char* data, size_t size, char* srcIP,
		char* destIP, char* routerIP, int ttl, int sourcePort, int destPort) {

	cout << "SENDING" << endl;

	unsigned int srcAddr = 0;
	inet_pton(AF_INET, srcIP, &srcAddr);

	unsigned int destAddr = 0;
	inet_pton(AF_INET, destIP, &destAddr);

	unsigned int routerAddr = 0;
	inet_pton(AF_INET, routerIP, &routerAddr);




	char* dataBuf = (char*)malloc(size);
	memcpy(dataBuf, data, size);

	packethdr* p = (packethdr*)malloc(sizeof(packethdr));
	//p->data = dataBuf;
	p->ip_header.ip_src.s_addr = srcAddr;
	p->ip_header.ip_dst.s_addr = destAddr;
	p->ip_header.ip_v = 4;
	p->ip_header.ip_hl = 5;
	p->ip_header.ip_len = 20;
	p->ip_header.ip_id = *id;
	p->ip_header.ip_off = 0;
	p->ip_header.ip_sum = 0;
	p->ip_header.ip_tos = 0;
	p->ip_header.ip_ttl = ttl;
	p->ip_header.ip_p = IPPROTO_UDP;

	p->udp_header.dest = destPort;
	p->udp_header.source = sourcePort;
	p->udp_header.check = 0;
	p->udp_header.len = size;

	//strncpy(p->datahdr.filename, filename.c_str(), FILENAME_LENGTH);

	cout << routerIP << endl;
	cout << "This translates to 101: " << routerAddr << endl;

	printPacket(p);

	(*id)++;
	int bytesSent = sendPacket(sock, p, data, routerAddr);



	delete dataBuf;

	return bytesSent;
}

int sendPacket(int sock, packethdr* p, char* data, unsigned long nextIP) {

	//int bufSize = sizeof(packethdr) + sizeof(dataheader) + p->header.udp_header.len;
	int bufSize = sizeof(packethdr) + p->udp_header.len;

	char* buffer = (char*)malloc(sizeof(packethdr) + p->udp_header.len);
	memcpy(buffer, p, sizeof(packethdr));
	memcpy(buffer + sizeof(packethdr), data, p->udp_header.len);

	int size = cs3516_send(sock, (char*) buffer, bufSize, nextIP);

	in_addr tmp;
	tmp.s_addr = nextIP;

	cout << endl << endl;
	cout << inet_ntoa(tmp) << endl;
	cout << endl << endl;
	//printPacket((packethdr*) buffer);

	delete buffer;

	return size;
}

void printPacket(packethdr* p) {
	printf("PACKET---------------------------------\n");
	printf("  IP HEADER----------------------------\n");
	printf("    Version       : %d \n", p->ip_header.ip_v);
	printf("    Header Length : %d \n", p->ip_header.ip_hl);
	printf("    Service Type  : %d \n", p->ip_header.ip_tos);
	printf("    Size          : %d \n", p->ip_header.ip_len);
	printf("    ID            : %d \n", p->ip_header.ip_id);
	printf("    Offset        : %d \n", p->ip_header.ip_off);
	printf("    TTL           : %d \n", p->ip_header.ip_ttl);
	printf("    Protocol      : %d \n", p->ip_header.ip_p);
	printf("    Check Sum     : %x \n", p->ip_header.ip_sum);
	printf("    Source IP     : %s \n", inet_ntoa(p->ip_header.ip_src));
	printf("    Dest IP       : %s \n", inet_ntoa(p->ip_header.ip_dst));

	printf("  UDP HEADER---------------------------\n");
	printf("    Source        : %d \n", p->udp_header.source);
	printf("    Destination   : %d \n", p->udp_header.dest);
	printf("    Length        : %d \n", p->udp_header.len);
	printf("    Checksum      : %d \n", p->udp_header.check);

	//printf("  Data---------------------------------\n");

	//char* test = (char*) malloc(p->udp_header.len);
	//memcpy(test, (p + sizeof(packethdr)), p->udp_header.len);
	//cout << test;

//	char* data = (char*) (p + sizeof(packethdr));
//	for (int i = 0; i < p->udp_header.len; i++) {
//		printf("%c", *(data + (i * sizeof(char))));
//	}
//	cout << endl;
	printf("END PACKET-----------------------------\n\n");
}

