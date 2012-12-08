/*
 * PacketQueue.cpp
 *
 *  Created on: Dec 7, 2012
 *      Author: cs3516
 */

#include <pthread.h>
#include <string.h>
#include <mutex>
#include <queue>

#include "PacketQueue.h"

namespace std {

void* print_message(void* queue);

PacketQueue::PacketQueue(int socket, unsigned int size, int dest, int source) {


	sock = socket;

	//TODO Remove once the delay list is populated
	//sendRate = delayList[source][dest];
	sendRate = 5;
	queueSize = size;
	running = false;

	queueLock = PTHREAD_MUTEX_INITIALIZER;

	this->thread = new pthread_t;
}

PacketQueue::~PacketQueue() {
	this->running = false;
	pthread_join(*(this->getThread()), 0);
}

pthread_t* PacketQueue::runQueue() {
	cout << "Starting thread... ";
	this->running = true;
	if (pthread_create(this->thread, NULL, (&(print_message)), (void*) this) == 0){
		cout << "Done." << endl;
		return this->thread;
	} else {
		cout << "Whoops!" << endl;
		return NULL;
	}
}

void* print_message(void* ptr) {
	PacketQueue* q = (PacketQueue*)ptr;
	packet* p = new packet;
	bool exiting = false;

	//char* buf = new char[MAX_PACKET_SIZE];

	while(!exiting) {
		//bzero(buf, MAX_PACKET_SIZE);

		while(q->getQueue()->empty()){
			if(!q->isRunning()) {
				exiting = true;
				break;
			}
			usleep(10);
		}

		pthread_mutex_lock(q->getQueueLock());
		packet* front = q->getQueue()->front();
		int packetSize = (front->header.udp_header.len + sizeof(packethdr));
//		pthread_mutex_unlock(q->getQueueLock());
//
//		//memcpy(p, front, packetSize);
//		pthread_mutex_lock(q->getQueueLock());
		int was = q->getQueue()->size();
		q->getQueue()->pop();
		int is = q->getQueue()->size();
		pthread_mutex_unlock(q->getQueueLock());

		cout << "Sending packet... " << endl;
		cout << "Queue size was " << was << endl;
		cout << "Queue size is now " << is << endl;
		int send = cs3516_send(q->getSocket(), (char*)p, packetSize, p->header.ip_header.ip_dst.s_addr);
		cout << "Done." << endl;
		sleep(q->sendRate);
	}
	cout.flush();
	return 0;
}
} /* namespace std */
