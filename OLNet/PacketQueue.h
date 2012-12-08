/*
 * packetQueue.h
 *
 *  Created on: Dec 7, 2012
 *      Author: cs3516
 */

#ifndef PACKETQUEUE_H_
#define PACKETQUEUE_H_

#include <queue>
#include <pthread.h>
#include <iostream>
#include <mutex>

#include "Packet.h"

namespace std {

class PacketQueue {
public:
	int queueSize;
	int sendRate;
	PacketQueue(int sock, unsigned int size, int dest, int source);
	virtual ~PacketQueue();
	pthread_t* runQueue();


	pthread_t* getThread() {
		return this->thread;
	}
	queue<packet*>* getQueue() {
		return &(this->q);
	}
	int getSocket() {
		return this->sock;
	}
	pthread_mutex_t* getQueueLock() {
		return &(this->queueLock);
	}
	bool isRunning(){
		return this->running;
	}
private:
	pthread_mutex_t queueLock;
	bool running;
	pthread_t* thread;
	queue<packet*> q;
	int sock;

};

} /* namespace std */

#endif /* PACKETQUEUE_H_ */

