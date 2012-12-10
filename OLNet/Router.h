/*
 * Router.h
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#ifndef ROUTER_H_
#define ROUTER_H_

#include "LPMTree.h"
#include "Packet.h"
#include <map>
#include <list>

int runRouter(in_addr* ip,
		int deviceID,
		int queueSize,
		std::LPMTree* lpmIn,
		std::PacketLogger* logger,
		std::map<int, std::map<int, int> >* delayList,
		std::map<uint32_t, int>* ipToDeviceID,
		std::map<int, std::list<int> >* deviceList);

#endif /* ROUTER_H_ */
