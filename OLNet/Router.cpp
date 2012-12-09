/*
 * Router.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <map>
#include <queue>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Router.h"
#include "Packet.h"
#include "LPMTree.h"
#include "cs3516sock.h"
#include "PacketQueue.h"
#include "PacketLogger.h"

using namespace std;

#define VERBOSE (0)

LPMTree* tree = new LPMTree();
PacketLogger* log = new PacketLogger;

int routerSock = -1;

static void terminate(int param) {
	cout << "\nTerminating router..." << endl;

	cout << "Deleting LPM Tree... ";
	delete (tree);
	cout << "Done." << endl;

	cout << "Closing log file... ";
	log->closeLogger();
	cout << "Done." << endl;

	cout << "Closing socket... ";
	close(routerSock);
	cout << "Done." << endl;

	exit(1);
}

int runRouter(in_addr* ip, int queueSize) {

	queue<packethdr> pQ;

	signal(SIGINT, terminate);
	signal(SIGABRT, terminate);
	signal(SIGTERM, terminate);

	in_addr addr;
	bool testing = false;
	if (testing) {
		tree->insert(7, (uint32_t) 0x30000000, 4);
		tree->insert(3, (uint32_t) 0x80000000, 2);
		tree->insert(6, (uint32_t) 0x70000000, 4);
		tree->insert(2, (uint32_t) 0x30000000, 2);
		tree->insert(100, (uint32_t) rand(), 32);
		tree->insert(100, (uint32_t) rand(), 32);
		tree->insert(100, (uint32_t) rand(), 31);
		tree->insert(100, (uint32_t) rand(), 31);
		tree->insert(12, (uint32_t) 0x19BA832C, 24);

		inet_aton("192.168.1.1", &addr);
		printf("%X\n", ntohl(addr.s_addr));
		tree->insert(21, ntohl(addr.s_addr), 24);
		//tree->insert(22, addr, 24);
		cout << endl;

		tree->print();

		cout << "This should be a '7' : " << tree->get((uint32_t) 0x30000000, 4)
				<< endl;
		cout << "This should be a '3' : " << tree->get((uint32_t) 0x80000000, 2)
				<< endl;
		cout << "This should be a '2' : " << tree->get((uint32_t) 0x30000000, 2)
				<< endl;
		cout << "This should be a '-1': " << tree->get((uint32_t) 0xC0000000, 2)
				<< endl;
		cout << "This should be a '12': "
				<< tree->get((uint32_t) 0x19BA832C, 24) << endl;

		char* fub = new char[16];
		inet_ntop(AF_INET, &addr, fub, 15);
		printf("%8X ", ntohl(addr.s_addr));
		printf("%15s", (inet_ntoa(addr)));
		cout << "\t" << tree->get(addr, 24) << endl;

		cout << endl;
		delete (tree);

		cout << endl;

		printf("Should be 32: %d\n",
				string("01010101010101010101010101010101").length());
	}

	tree->print();

	cout << endl << "---------- Router Mode Started" << endl;

	cout << "Opening log file... ";
	log->initLogger();
	cout << "Done." << endl;

	cout << "Opening socket... ";
	routerSock = create_cs3516_socket();
	if (routerSock < 0) {
		cout << "Socket creation failed. Exiting." << endl;
		abort();
	}
	cout << "Done." << endl;

	char* buf = new char[MAX_PACKET_SIZE];
	packethdr* p;
	char* data;

	timeval t { 0 };
	t.tv_usec = 10;
	fd_set set;
	FD_SET(routerSock, &set);

	//PacketQueue q = new PacketQueue(queueSize, delayList[1][2]);
	int toDev = 0;
	int fromDev = 0;
	PacketQueue* q = new PacketQueue(routerSock, queueSize, toDev, fromDev, log);

	cout << "runQueue()" << endl;
	q->runQueue();
	cout << "returned" << endl;

	packethdr* newPacket = new packethdr;

	bool selecting = true;
	while (1) {
		bzero(buf, MAX_PACKET_SIZE);

		timeval t2;
		fd_set set2;

		cout << "Starting select..." << endl;
		selecting = true;
		while (selecting) {
			memcpy(&t2, &t, sizeof(timeval));
			memcpy(&set2, &set, sizeof(fd_set));
			int test = select(routerSock + 1, &set2, NULL, NULL, &t2);
			//cout << test << endl;
			if (test == -1) {
				perror("");
				exit(1);
			}
			if (test > 0) {
				selecting = false;
			}
		}

		int recvBytes = cs3516_recv(routerSock, buf, MAX_PACKET_SIZE);
		if (recvBytes > 0) {
			p = (packethdr*) buf;
			data = (char*) (p + sizeof(packethdr));
			if (VERBOSE)
				printPacket((packethdr*)p);

			// TODO Perform LPM Lookup
			// If returns -1, drop packet
			if (false) {
				globalLog.logPacket(p, globalLog.NO_ROUTE_TO_HOST);
				continue;
			}

			// If this is the last hop for the packet, drop it
			if (p->ip_header.ip_ttl == 1) {
				log->logPacket(p, globalLog.TTL_EXPIRED);
				continue;
			}

			// If there is no space for the packet, drop it
			if (q->getQueue()->size() >= q->queueSize) {
				log->logPacket(p, globalLog.MAX_SENDQ_EXCEEDED);
				continue;
			}

			newPacket = (packethdr*)malloc(recvBytes);
			memcpy(newPacket, buf, recvBytes);

			cout << "NEW PACKET CREATED!" << endl;

			pthread_mutex_lock(q->getQueueLock());
			q->enqueue(newPacket);
			cout << "\tENQUEUE " << q->getQueue()->size() << endl;
			pthread_mutex_unlock(q->getQueueLock());

			//int send = cs3516_send(routerSock, buf, recvBytes, p->ip_header.ip_dst.s_addr);

			usleep(100);
		} else {
			cout << "WHAT THE HELL, SELECT? THERE WAS NO DATA." << endl;
		}
	}
	return 0;
}

