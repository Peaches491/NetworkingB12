/*
 * Host.h
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#ifndef HOST_H_
#define HOST_H_

#include <map>
#include <list>
#include <strings.h>

struct fileData{
	int deviceID;
	int filesize;
	int packetCount;
	int packets;
	int totalData;
	int totalDataSansHeaders;
	std::string filename;
	std::list<int> missingPackets;
	FILE* file;
};

int runHost(in_addr* ip,
		int deviceID,
		int ttl,
		std::map<int, uint32_t>* idToRealIP,
		std::map<int, uint32_t>* idToOverlayIP,
		std::map<int, int>* hostToRouter,
		std::map<int, std::map<int, int> >* delayList,
		std::map<uint32_t, int>* overlayIPToDeviceID);

int getFileSize(FILE* fd);

#endif /* HOST_H_ */
