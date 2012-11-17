/*
 * OLPacket.h
 *
 *  Created on: Nov 17, 2012
 *      Author: cs3516
 */

#ifndef OLPACKET_H_
#define OLPACKET_H_


#include <netinet/ip.h>
#include <netinet/udp.h>
#include "cs3516sock.h"

typedef struct _packethdr {

	struct ip ip_header;
	struct udphdr udp_header;
	unsigned int dataSize;

} packethdr;

typedef struct _packet {

	packethdr header;
	char* data;

} packet;


int sendPacket(int sock, packet* p, unsigned long nextIP){

	int bufSize = sizeof(packethdr) + p->header.dataSize;



	char* buffer = new char[bufSize];
	memcpy(buffer, p, sizeof(packethdr));
	memcpy(buffer + sizeof(packethdr), p->data, p->header.dataSize);

	std::cout << "Buff: " << bufSize << " bytes" << std::endl;
	std::cout << buffer+sizeof(packethdr) << std::endl;

	int size = cs3516_send(sock, (char*)buffer, bufSize, nextIP);

	//delete buffer;

	return size;
}


#endif /* OLPACKET_H_ */
