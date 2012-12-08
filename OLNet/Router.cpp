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
#include <mutex>
#include <sstream>
#include <fstream>
#include <iostream>


#include "Router.h"
#include "Packet.h"
#include "LPMTree.h"
#include "cs3516sock.h"
#include "PacketQueue.h"

using namespace std;

#define VERBOSE (0)

LPMTree* tree = new LPMTree();
ofstream log;
std::map<int, string> typeString;

static void terminate(int param) {
	cout << "\nTerminating router..." << endl;

	cout << "Deleting LPM Tree... ";
	delete (tree);
	cout << "Done." << endl;

	cout << "Closing log file... ";
	if (log != NULL)
		log.close();
	cout << "Done." << endl;

	exit(1);
}

int runRouter(in_addr* ip, int queueSize) {

	queue<packet> pQ;

	signal(SIGINT, terminate);
	signal(SIGABRT, terminate);
	signal(SIGTERM, terminate);

	typeString[TTL_EXPIRED] = string("TTL_EXPIRED");
	typeString[MAX_SENDQ_EXCEEDED] = string("MAX_SENDQ_EXCEEDED");
	typeString[NO_ROUTE_TO_HOST] = string("NO_ROUTE_TO_HOST");
	typeString[SENT_OKAY] = string("SENT_OKAY");

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

		cout << "This should be a '7' : " <<
				tree->get((uint32_t) 0x30000000, 4) << endl;
		cout << "This should be a '3' : " <<
				tree->get((uint32_t) 0x80000000, 2) << endl;
		cout << "This should be a '2' : " <<
				tree->get((uint32_t) 0x30000000, 2)<< endl;
		cout << "This should be a '-1': " <<
				tree->get((uint32_t) 0xC0000000, 2)<< endl;
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
	log.open("ROUTER_control.txt", ios::out);
	if (log == NULL) {
		cout << "Log file could not be opened. Exiting." << endl;
		abort();
	}
	cout << "Done." << endl;

	cout << "Opening socket... ";
	int sock = create_cs3516_socket();
	if (sock < 0) {
		cout << "Socket creation failed. Exiting." << endl;
		abort();
	}
	cout << "Done." << endl;

	char* buf = new char[MAX_PACKET_SIZE];
	packet* p;
	char* data;
	timeval t {0};
	t.tv_usec = 10;
	fd_set set;
	FD_SET(sock, &set);

	//PacketQueue q = new PacketQueue(queueSize, delayList[1][2]);
	int toDev = 0;
	int fromDev = 0;
	PacketQueue* q = new PacketQueue(sock, queueSize, toDev, fromDev);

	cout << "runQueue()" << endl;
	q->runQueue();
	cout << "returned" << endl;

	packet* newPacket = new packet;

	while (1) {
		bzero(buf, MAX_PACKET_SIZE);

		timeval t2 {0};
		fd_set set2 {0};

		cout << "Starting select..." << endl;
		bool selecting = true;
		while (selecting) {
			memcpy(&t2, &t, sizeof(timeval));
			memcpy(&set2, &set, sizeof(fd_set));
			int test = select(sock + 1, &set2, NULL, NULL, &t2);
			//cout << test << endl;
			if(test == -1){
				perror("");
				exit(1);
			}
			if (test > 0) {
				selecting = false;
			}
		}

		int recv = cs3516_recv(sock, buf, MAX_PACKET_SIZE);
		p = (packet*) buf;
		data = (char*) (p + sizeof(packethdr));
		if (VERBOSE) printPacket(p);

		// TODO Perform LPM Lookup
		// If returns -1, drop packet
		if(false){
			logPacket(p, NO_ROUTE_TO_HOST);
			continue;
		}

		// If this is the last hop for the packet, drop it
		if (p->header.ip_header.ip_ttl == 1) {
			logPacket(p, TTL_EXPIRED);
			continue;
		}

		// If there is no space for the packet, drop it
		if(q->getQueue()->size() >= q->queueSize){
			logPacket(p, MAX_SENDQ_EXCEEDED);
			continue;
		}

		newPacket = new packet;
		memcpy(newPacket, p, recv);

		pthread_mutex_lock(q->getQueueLock());
		q->getQueue()->push(newPacket);
		cout << "ENQUEUE " << q->getQueue()->size() << endl;
		pthread_mutex_unlock(q->getQueueLock());

		//int send = cs3516_send(sock, buf, recv,	p->header.ip_header.ip_dst.s_addr);

		logPacket(p, SENT_OKAY, p->header.ip_header.ip_dst);
		usleep(100);
	}
	return 0;
}

void logPacket(packet* p, LogType type) {
	in_addr adr { 0 };
	logPacket(p, type, adr);
}

void logPacket(packet* p, LogType type, in_addr nextHop) {

	char* delim = " ";
	char* str = new char[100];



	if (type == SENT_OKAY){
		sprintf(str, "%ld%s%s%s%s%s%d%s%s",
					(long int)time(NULL), delim,
					inet_ntoa(p->header.ip_header.ip_src), delim,
					inet_ntoa(p->header.ip_header.ip_dst), delim,
					p->header.ip_header.ip_id, delim,
					(typeString[type]).c_str(), delim, inet_ntoa(nextHop));
	} else {
		sprintf(str, "%ld%s%s%s%s%s%d%s%s", (long int) time(NULL), delim,
				inet_ntoa(p->header.ip_header.ip_src), delim,
				inet_ntoa(p->header.ip_header.ip_dst), delim,
				p->header.ip_header.ip_id, delim, (typeString[type]).c_str());
	}


	cout << str << endl;
	log << str << endl;
}
