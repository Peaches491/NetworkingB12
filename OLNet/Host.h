/*
 * Host.h
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#ifndef HOST_H_
#define HOST_H_

#include <map>

int runHost(in_addr* ip,
		int deviceID,
		int ttl,
		std::map<int, uint32_t>* idToRealIP,
		std::map<int, int>* hostToRouter,
		std::map<int, std::map<int, int> >* delayList);

int getFileSize(FILE* fd);

#endif /* HOST_H_ */
