/*
 * Packet.h
 *
 *  Created on: Nov 17, 2012
 *      Author: cs3516
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <string>
#include "cs3516sock.h"

#define FILENAME_LENGTH (32)
#define MAX_PAYLOAD (1000)
//#define MAX_PAYLOAD (1000) - sizeof(dataheader)
#define MAX_PACKET_SIZE (sizeof(packethdr) + MAX_PAYLOAD)

///////////////////////// Types
typedef struct _packethdr {

	struct ip ip_header;
	struct udphdr udp_header;
	//unsigned int dataSize;

} packethdr;

//typedef struct _dataheader {
//	char filename[FILENAME_LENGTH];
//	unsigned int offset;
//} dataheader;

typedef struct _packet {

	packethdr header;
	//dataheader datahdr;
	char* data;

} packet;

///////////////////////// Function Definitions
int createAndSendPacket(int sock, int* id, char* data, size_t size, char* srcIP, char* destIP,
		char* routerIP, int ttl, std::string filename);
void printPacket(packet* p);
int sendPacket(int sock, packet* p, unsigned long nextIP);

///////////////////////// Functions

#endif /* OLPACKET_H_ */
