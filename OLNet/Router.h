/*
 * Router.h
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#ifndef ROUTER_H_
#define ROUTER_H_

#include "Packet.h"
#include <map>

enum LogType {TTL_EXPIRED,
	MAX_SENDQ_EXCEEDED,
	NO_ROUTE_TO_HOST,
	SENT_OKAY};

void logPacket(packet* p, LogType type);
void logPacket(packet* p, LogType type, in_addr nextHop);
int runRouter(in_addr* ip, int queueSize);

#endif /* ROUTER_H_ */
