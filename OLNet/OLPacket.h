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


typedef struct _packet {

	struct ip ip_header;
	struct udphdr udp_header;
	char* data;

} packet;




#endif /* OLPACKET_H_ */
