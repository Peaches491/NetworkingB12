//============================================================================
// Name        : OLRouter.cpp
// Author      : Daniel Miller and Kevin Janesch
// Description :
//============================================================================

#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include "Router.h"
#include "Host.h"
#include "Packet.h"
#include "cs3516sock.h"

using namespace std;

int main(int argc, char* argv[]) {

	if (argc < 2) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
		std::cout << "Usage is -r (for router mode) -h (for host mode)" << endl; // Inform the user of how to use the program
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

	char* file = "";
	char* destIP = "";
	char* routerIP = "";

	std: cout << "" << endl;

	for (int i = 1; i < argc; i++) {
		if ((i + 1) != argc) {

			//File
			if (strcmp(argv[i], "-f") == 0) {
				file = argv[i + 1];

				//Destination IP
			} else if (strcmp(argv[i], "-d") == 0) {
				destIP = argv[i + 1];

				//Max Users
			} else if (strcmp(argv[i], "-r") == 0) {
				routerIP = argv[i + 1];
			}
		}
	}

	if (router == true) {
		return runRouter(file);
	} else {
		return runHost(file, destIP, routerIP);
	}
}

