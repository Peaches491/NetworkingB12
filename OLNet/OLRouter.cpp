//============================================================================
// Name        : OLRouter.cpp
// Author      : Daniel Miller and Kevin Janesch
// Description :
//============================================================================

#include <iostream>
#include "OLPacket.h"
using namespace std;

int main(int argc, char* argv[]) {

	if (argc < 2) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
		std::cout << "Usage is -r (for router mode) -h (for host mode)"; // Inform the user of how to use the program
		return -1;
	}

	bool router = false;

	for (int i = 1; i < argc; i++) {

		if ((i) != argc) {
			// Host Tag
			if (strcmp(argv[i], "-h") == 0) {
				router = false;
				// Router Tag
			} else if (strcmp(argv[i], "-r") == 0) {
				router = true;
			}
		}
	}

	if (router == true) {
		cout << "---------- Router Mode Started" << endl;

//		struct addrinfo hints;
//		struct addrinfo* res;
		char* testWord = (char*) "0123456789";

		packet* p = new packet;
		p->data = testWord;
		p->header.dataSize = strlen(p->data) + 1;



		cout << endl;
		int sock = create_cs3516_socket();
		int sent = sendPacket(sock, p, 0);
		cout << "Sent: " << sent << " bytes" << endl;

//		char* buf = new char[1000];
//		int recv = cs3516_recv(sock, buf, 1000);
//		cout << "Recv: " << recv << " bytes" << endl;
//
//		cout << "Header Size: " << sizeof(packethdr) << " bytes" << endl;
//		cout << "Data Size: " << ((packet*) buf)->header.dataSize << " bytes"
//				<< endl;
//
//		char* data = buf + sizeof(packethdr);
//		cout << data << endl;

		return 0;
	} else {
		cout << "---------- Host Mode Started" << endl;

		int sock = create_cs3516_socket();

		char* buf = new char[1000];
		int recv = cs3516_recv(sock, buf, 1000);

		cout << endl;

		cout << "Recv: " << recv << " bytes" << endl;

		cout << "Header Size: " << sizeof(packethdr) << " bytes" << endl;
		cout << "Data Size: " << ((packet*) buf)->header.dataSize << " bytes"
				<< endl;

		char* data = buf + sizeof(packethdr);
		cout << data << endl;

		return 0;
	}
}
