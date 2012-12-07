/*
 * OLPacket.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "cs3516sock.h"
#include "Packet.h"


int createAndSendPacket(int sock, int* id, char* data, size_t size, char* srcIP, char* destIP, char* routerIP, int ttl, std::string filename) {
	unsigned int srcAddr = 0;
	inet_pton(AF_INET, srcIP, &srcAddr);

	unsigned int destAddr = 0;
	inet_pton(AF_INET, destIP, &destAddr);

	unsigned int routerAddr = 0;
	inet_pton(AF_INET, routerIP, &routerAddr);

	char* dataBuf = new char[size];
	memcpy(dataBuf, data, size);

	packet* p = new packet;
	p->data = dataBuf;
	p->header.ip_header.ip_src.s_addr = srcAddr;
	p->header.ip_header.ip_dst.s_addr = destAddr;
	p->header.ip_header.ip_v = 4;
	p->header.ip_header.ip_hl = 5;
	p->header.ip_header.ip_len = 20;
	p->header.ip_header.ip_id = *id;
	p->header.ip_header.ip_off = 0;
	p->header.ip_header.ip_sum = 0;
	p->header.ip_header.ip_ttl = ttl;
	p->header.ip_header.ip_p = IPPROTO_UDP;

	p->header.udp_header.dest = MYPORT;
	p->header.udp_header.source = MYPORT;
	p->header.udp_header.check = 0;
	p->header.udp_header.len = size;


	//strncpy(p->datahdr.filename, filename.c_str(), FILENAME_LENGTH);

	(*id)++;
	return sendPacket(sock, p, routerAddr);
}

int sendPacket(int sock, packet* p, unsigned long nextIP) {

	//int bufSize = sizeof(packethdr) + sizeof(dataheader) + p->header.udp_header.len;
	int bufSize = sizeof(packethdr) + p->header.udp_header.len;

	char* buffer = new char[bufSize];
	memcpy(buffer, p, sizeof(packethdr));
	memcpy(buffer + sizeof(packethdr), p->data, p->header.udp_header.len);

	int size = cs3516_send(sock, (char*) buffer, bufSize, nextIP);

	printPacket((packet*)buffer);

	//delete buffer;

	return size;
}

void printPacket(packet* p) {
	printf("PACKET---------------------------------\n");
	printf("  IP HEADER----------------------------\n");
	printf("    Version       : %d \n", p->header.ip_header.ip_v);
	printf("    Header Length : %d \n", p->header.ip_header.ip_hl);
	printf("    Service Type  : %d \n", p->header.ip_header.ip_tos);
	printf("    Size          : %d \n", p->header.ip_header.ip_len);
	printf("    ID            : %d \n", p->header.ip_header.ip_id);
	printf("    Offset        : %d \n", p->header.ip_header.ip_off);
	printf("    TTL           : %d \n", p->header.ip_header.ip_ttl);
	printf("    Protocol      : %d \n", p->header.ip_header.ip_p);
	printf("    Check Sum     : %x \n", p->header.ip_header.ip_sum);
	printf("    Source IP     : %s \n", inet_ntoa(p->header.ip_header.ip_src));
	printf("    Dest IP       : %s \n", inet_ntoa(p->header.ip_header.ip_dst));

	printf("  UDP HEADER---------------------------\n");
	printf("    Source        : %d \n", p->header.udp_header.source);
	printf("    Destination   : %d \n", p->header.udp_header.dest);
	printf("    Length        : %d \n", p->header.udp_header.len);
	printf("    Checksum      : %d \n", p->header.udp_header.check);

	printf("  Data---------------------------------\n");

	char* data = (char*)(((void*)p) + sizeof(packethdr));
	for (int i = 0; i < p->header.udp_header.len; i++) {
		std::cout << data[i];
	}
	std::cout << std::endl;
	printf("END PACKET-----------------------------\n\n");
}

