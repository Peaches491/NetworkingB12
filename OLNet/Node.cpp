//============================================================================
// Name        : OLRouter.cpp
// Author      : Daniel Miller and Kevin Janesch
// Description :
//============================================================================

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include "Router.h"
#include "Delays.h"
#include "Host.h"
#include "Packet.h"
#include "cs3516sock.h"

using namespace std;

void getIP(in_addr* result);

int main(int argc, char* argv[]) {

	char* file = "";
	char* destIP = "";
	char* routerIP = "";

	// Check the value of argc.
	//    If not enough parameters have been passed,
	//    inform user and exit.
	if (argc < 2) {
		std::cout << "Usage is -r (for router mode) -h (for host mode)" << endl;
		return -1;
	}

	bool router = false;

	in_addr ip;
	getIP(&ip);
	cout << "Detected IP Address is: " << inet_ntoa(ip) << endl;

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

	//TODO map machine numbers to various datas
	// example1: 3 1 100 2 110
	delayList[1][2] = 100;
	delayList[2][1] = 110;
	// example1: 4 1 1000 1.2.3.0/24 4 2000
	delayList[1][4] = 1000;
	delayList[4][1] = 2000;

	if (router == true) {
		return runRouter(&ip, 10); // TODO 10 is default Queue Length
	} else {
		return runHost(&ip, file, destIP, routerIP, 3); // TODO 3 is default TTL
	}
}

void getIP(in_addr* result) {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	//struct in_addr* result = new in_addr();
	struct in_addr* tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			//result = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr
			tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			memcpy(result, tmpAddrPtr, sizeof(in_addr));
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
		}
	}
	if (ifAddrStruct != NULL)
		freeifaddrs(ifAddrStruct);
	//return result;
}
