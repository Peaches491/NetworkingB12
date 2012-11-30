/*
 * Router.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include "Router.h"
#include "Packet.h"
#include "cs3516sock.h"

using namespace std;

int runRouter(int argc, char* argv[]) {
	cout << "---------- Router Mode Started" << endl;

	int sock = create_cs3516_socket();
	char* buf = new char[MAX_PACKET_SIZE];

	while(1){
		bzero(buf, MAX_PACKET_SIZE);
		int recv = cs3516_recv(sock, buf, MAX_PACKET_SIZE);
		//cout << "I got some data! " << recv << endl;
		printPacket((packet*)buf);
	}
	////////////////////////////////////

//	char* testWord = (char*) "0123456789";
//
//	packet* p = new packet;
//	p->header.ip_header.ip_ttl = 12;
//	p->header.ip_header.ip_v = 4;
//	p->header.ip_header.ip_hl = 5;
//
//
//	if (p->header.ip_header.ip_ttl != 0) {
//		p->header.ip_header.ip_ttl -= 1;
//	} else {
//		dropPacket(p);
//	}
//	p->data = testWord;
//	p->header.udp_header.len = strlen(p->data) + 1;
//	cout << endl;
//
//	unsigned int addr = 0;
//	inet_pton(AF_INET, "192.168.132.164", &addr);
//
//	int sent = sendPacket(sock, p, addr);
//
//	cout << "Sent: " << sent << " bytes" << endl;

	return 0;
}


void dropPacket(packet* p) {
	cout << "DROPPING PACKET" << endl;
	printPacket(p);
}



