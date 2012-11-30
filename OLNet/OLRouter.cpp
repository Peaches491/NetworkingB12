//============================================================================
// Name        : OLRouter.cpp
// Author      : Daniel Miller and Kevin Janesch
// Description :
//============================================================================

#include <iostream>
#include <arpa/inet.h>
#include "OLPacket.h"
using namespace std;

void dropPacket(packet* p);
void printPacket(packet* p);
int runRouter(int argc, char* argv[]);
int runHost(int argc, char* argv[]);

int main(int argc, char* argv[]) {

	if (argc < 2) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
		std::cout << "Usage is -r (for router mode) -h (for host mode)"; // Inform the user of how to use the program
		return -1;
	}

	bool router = false;

	// Host Tag
	if (strcmp(argv[1], "-h") == 0) {
		router = false;
		// Router Tag
	} else if (strcmp(argv[1], "-r") == 0) {
		router = true;
	} else {
		cout
				<< "ERROR: Argument 1 must specify either -r (for router mode) -h (for host mode)"
				<< endl;
		cout << endl;
		return -1;
	}

	if (router == true) {
		return runRouter(argc, argv);
	} else {
		return runHost(argc, argv);
	}
}

int runRouter(int argc, char* argv[]) {
	cout << "---------- Router Mode Started" << endl;

	//		struct addrinfo hints;
	//		struct addrinfo* res;
	char* testWord = (char*) "0123456789";

	packet* p = new packet;
	p->header.ip_header.ip_ttl = 12;
	p->header.ip_header.ip_v = 4;
	p->header.ip_header.ip_hl = 5;


	if (p->header.ip_header.ip_ttl != 0) {
		p->header.ip_header.ip_ttl -= 1;
	} else {
		dropPacket(p);
	}
	p->data = testWord;
	p->header.dataSize = strlen(p->data) + 1;
	cout << endl;

	int sock = create_cs3516_socket();

	unsigned int addr = 0;
	inet_pton(AF_INET, "192.168.132.164", &addr);

	int sent = sendPacket(sock, p, addr);

	cout << "Sent: " << sent << " bytes" << endl;

	return 0;
}

int runHost(int argc, char* argv[]) {
	cout << "---------- Host Mode Started" << endl;

	int sock = create_cs3516_socket();

	char* buf = new char[1000];

	while (1) {
		bzero(buf, 1000);
		int recv = cs3516_recv(sock, buf, 1000);

		packet* p = ((packet*) buf);

		cout << endl;
		cout << "Recv: " << recv << " bytes" << endl;

		cout << "Header Size: " << sizeof(packethdr) << " bytes" << endl;
		cout << "Data Size: " << ((packet*) buf)->header.dataSize << " bytes"
				<< endl;

		printPacket(p);

		char* data = buf + sizeof(packethdr);
		cout << data << endl;
	}
	return 0;
}

void dropPacket(packet* p) {
	cout << "DROPPING PACKET" << endl;
	printPacket(p);
}

void printPacket(packet* p) {
//	cout << "PACKET INFO: " << endl;
//	cout << "\tOL Source: " << inet_ntoa(p->header.ip_header.ip_src) << endl;
//	cout << "\tOL Dest:   " << inet_ntoa(p->header.ip_header.ip_dst) << endl;
//	cout << "\tOL TTL:    " << (unsigned int) p->header.ip_header.ip_ttl
//			<< endl;
	printf("PACKET---------------------------------\n",
			p->header.ip_header.ip_v);
	printf("\tVersion       : %d \n", p->header.ip_header.ip_v);
	printf("\tHeader Length : %d \n", p->header.ip_header.ip_hl);
	printf("\tService Type  : %d \n", p->header.ip_header.ip_tos);
	printf("\tSize          : %d \n", p->header.ip_header.ip_len);
	printf("\tID            : %d \n", p->header.ip_header.ip_id);
	//printf("\tFlags         : %x \n", p->header.ip_header);
	printf("\tOffset        : %d \n", p->header.ip_header.ip_off);
	printf("\tTTL           : %d \n", p->header.ip_header.ip_ttl);
	printf("\tProtocol      : %d \n", p->header.ip_header.ip_p);
	printf("\tCheck Sum     : %x \n", p->header.ip_header.ip_sum);
	printf("\tSource IP     : %s \n", inet_ntoa(p->header.ip_header.ip_src));
	printf("\tDest IP       : %s \n", inet_ntoa(p->header.ip_header.ip_dst));
}
