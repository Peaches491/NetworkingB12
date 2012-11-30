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


int createAndSendPacket(int sock, char* data, size_t size, char* ip) {
	unsigned int addr = 0;
	inet_pton(AF_INET, ip, &addr);

	char* dataBuf = new char[size];
	memcpy(dataBuf, data, size);

	packet* p = new packet;
	p->data = dataBuf;
	p->header.udp_header.len = size;
	return sendPacket(sock, p, addr);
}

int sendPacket(int sock, packet* p, unsigned long nextIP) {

	int bufSize = sizeof(packethdr) + p->header.udp_header.len;

	char* buffer = new char[bufSize];
	memcpy(buffer, p, sizeof(packethdr));
	memcpy(buffer + sizeof(packethdr), p->data, p->header.udp_header.len);

	std::cout << "Buff: " << bufSize << " bytes" << std::endl;
	std::cout << buffer + sizeof(packethdr) << std::endl;

	int size = cs3516_send(sock, (char*) buffer, bufSize, nextIP);

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

	char* data = (char*)((char*)p + sizeof(packethdr));
	std::cout << data << std::endl;
	printf("END PACKET-----------------------------\n\n");
}

