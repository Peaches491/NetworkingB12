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
#include "cs3516sock.h"

#define MAX_PAYLOAD (1000)
#define MAX_PACKET_SIZE (sizeof(packethdr) + MAX_PAYLOAD)


///////////////////////// Types
typedef struct _packethdr {

	struct ip ip_header;
	struct udphdr udp_header;
	//unsigned int dataSize;

} packethdr;

typedef struct _packet {

	packethdr header;
	char* data;

} packet;

///////////////////////// Function Definitions
int createAndSendPacket(int sock, char* data, size_t size, char* ip);
void printPacket(packet* p);
int sendPacket(int sock, packet* p, unsigned long nextIP);

///////////////////////// Functions


#endif /* OLPACKET_H_ */
