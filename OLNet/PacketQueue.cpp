/*
 * PacketQueue.cpp
 *
 *  Created on: Dec 7, 2012
 *      Author: cs3516
 */

#include <pthread.h>
#include <string.h>
#include <queue>

#include "PacketQueue.h"
#include "PacketLogger.h"

namespace std {

void* print_message(void* queue);

PacketQueue::PacketQueue(int socket, unsigned int size, int dest, int source,
		PacketLogger* logPtr, map<int, map<int, int> >* delayList, uint32_t _nextHop) {
	sock = socket;
	sendRate = (*delayList)[source][dest];
	queueSize = size;
	running = false;
	queueLock = PTHREAD_MUTEX_INITIALIZER;
	log = logPtr;
	nextHop = _nextHop;

	this->thread = new pthread_t;
}

PacketQueue::~PacketQueue() {
	this->running = false;
	pthread_join(*(this->getThread()), 0);
}

void PacketQueue::enqueue(packethdr* p) {
	pthread_mutex_lock(this->getQueueLock());
	this->getQueue()->push(p);
	pthread_mutex_unlock(this->getQueueLock());
}

pthread_t* PacketQueue::runQueue() {
	cout << "Starting thread... ";
	this->running = true;
	if (pthread_create(this->thread, NULL, (&(print_message)), (void*) this)
			== 0) {
		cout << "Done." << endl;
		return this->thread;
	} else {
		cout << "Whoops!" << endl;
		return NULL;
	}
}

void* print_message(void* ptr) {
	PacketQueue* q = (PacketQueue*) ptr;
	packethdr* p = new packethdr;
	bool exiting = false;
	bool verbose = false;

	char* buf = new char[MAX_PACKET_SIZE];

	while (!exiting) {
		bzero(buf, MAX_PACKET_SIZE);

		while (q->getQueue()->empty()) {
			if (!q->isRunning()) {
				exiting = true;
				break;
			}
			usleep(10);
		}

		pthread_mutex_lock(q->getQueueLock());
		packethdr* front = q->getQueue()->front();

		int packetSize = (front->udp_header.len + sizeof(packethdr));

		memcpy(buf, (q->getQueue()->front()), MAX_PACKET_SIZE);

		int was = q->getQueue()->size();
		q->getQueue()->pop();
		int is = q->getQueue()->size();
		pthread_mutex_unlock(q->getQueueLock());

		p = front;

		if (verbose)
			cout << "Sending packet... " << endl;
		if (verbose)
			cout << "Queue size was " << was << endl;
		if (verbose)
			cout << "Queue size is now " << is << endl;

		int send = cs3516_send(q->getSocket(), (char*) p, packetSize,
				q->nextHop);

		if (verbose)
			cout << "Bytes sent " << send << endl;

		in_addr tmp;
		tmp.s_addr = q->nextHop;
		q->log->logPacket(p, SENT_OKAY, tmp);

		if (verbose)
			cout << "Done." << endl << endl;
		//printPacket((packethdr*)p);

		usleep(q->sendRate * 1000);
	}
	cout.flush();
	return 0;
}
} /* namespace std */
