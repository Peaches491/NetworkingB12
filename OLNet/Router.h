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
int runRouter(int argc, char* argv[]);

#endif /* ROUTER_H_ */
