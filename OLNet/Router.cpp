/*
 * Router.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/time.h>
#include <map>
#include <queue>

#include "LPMTree.h"
#include "Router.h"
#include "Packet.h"
#include "cs3516sock.h"

using namespace std;

#define VERBOSE (0)

LPMTree* tree = new LPMTree();
ofstream log;
std::map<int, const char*> typeString;

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

	typeString[TTL_EXPIRED] = "TTL_EXPIRED";
	typeString[MAX_SENDQ_EXCEEDED] = (const char*) "MAX_SENDQ_EXCEEDED";
	typeString[NO_ROUTE_TO_HOST] = (const char*) "NO_ROUTE_TO_HOST";
	typeString[SENT_OKAY] = (const char*) "SENT_OKAY";

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
	timeval t;
	t.tv_usec = 10;
	fd_set set;
	FD_SET(sock, &set);

	while (1) {
		bzero(buf, MAX_PACKET_SIZE);

		timeval t2;
		fd_set set2;
		bool selecting = true;

		while (selecting) {
			memcpy(&t2, &t, sizeof(timeval));
			memcpy(&set2, &set, sizeof(fd_set));
			int test = select(sock + 1, &set2, NULL, NULL, &t2);
			if (test > 0) {
				selecting = false;
				//cout << test << endl;
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
		if(pQ.size() >= queueSize){
			logPacket(p, MAX_SENDQ_EXCEEDED);
			continue;
		}
		packet pac;
		pQ.push(pac);

		int send = cs3516_send(sock, buf, recv,
				p->header.ip_header.ip_dst.s_addr);
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

	string delim = string(" ");

	cout << time(NULL) << delim;
	cout << inet_ntoa(p->header.ip_header.ip_src) << delim;
	cout << inet_ntoa(p->header.ip_header.ip_dst) << delim;
	cout << (p->header.ip_header.ip_id) << delim;
	cout << typeString[type];
	if (type == SENT_OKAY)
		cout << delim << inet_ntoa(nextHop);
	cout << endl;
	cout.flush();

	log << time(NULL) << delim;
	log << inet_ntoa(p->header.ip_header.ip_src) << delim;
	log << inet_ntoa(p->header.ip_header.ip_dst) << delim;
	log << (p->header.ip_header.ip_id) << delim;
	log << typeString[type];
	if (type == SENT_OKAY)
		log << delim << inet_ntoa(nextHop);
	log << endl;
	log.flush();
}
