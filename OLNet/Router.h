/*
 * Router.h
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#ifndef ROUTER_H_
#define ROUTER_H_

#include "Packet.h"

void dropPacket(packet* p);
int runRouter(char* configFile);

#endif /* ROUTER_H_ */
