/*
 * FileSender.h
 *
 *  Created on: Dec 10,
 2012
 *      Author: cs3516
 */

#ifndef FILESENDER_H_
#define FILESENDER_H_

#include <netinet/in.h>

namespace std {

struct data {
	int sock;
	int* id;
	char* file;
	int delay;
	char* hostOverlayIPString;
	char* destRealIPString;
	char* routerRealIPString;
	int ttl;
	int sourcePort;
	int destPort;
	int deviceID;
	int routerID;

};

class FileSender {
public:
	FileSender(data);
	pthread_t* send();
	virtual ~FileSender();
	int sock;
	int* id;
	char* file;
	char* hostOverlayIPString;
	char* destRealIPString;
	char* routerRealIPString;
	int ttl;
	int sourcePort;
	int destPort;
	int deviceID;
	int routerID;
	int delay;
	bool running;
	pthread_t* thread;
};

} /* namespace std */
#endif /* FILESENDER_H_ */
