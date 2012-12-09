//============================================================================
// Name        : OLRouter.cpp
// Author      : Daniel Miller and Kevin Janesch
// Description :
//============================================================================

#include <iostream>
#include <fstream>
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
int maskGen(int nbits);

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
	int queueLength = 10;
	int TTL = 10;
	if(1){
		ifstream f;
		f.open("config.txt", ios::in|ios::binary);
		cout<<"opening 'config.txt'...."<<endl;

		//seek to the end of the file
		f.seekg(0, ios::end);

		//file is open and not empty
		if(f.good() && f.tellg()>0){
			//hop back to the beginning
			f.seekg(0, ios::beg);

			cout<<"... opened. Parsing for config information..."<<endl;

			while(!f.eof()){
				char line[100];
				int tokenCount = 0;
				char* token;
				int parsedToken[6];
				in_addr tmpaddr;
				uint8_t type = 0;
				//cout<<"pulling a line."<<endl;
				f.getline(line, 99);			//pull a line from the file

				//empty line (could be whitespace, could be EOF)
				// if it's EOF the do/while condition catches it
				if(!f.gcount()) continue;

				cout<< "got tokens:  ";

				token = strtok(line, " ");

				do{
					//pull tokens out of it
					cout<< token<< ",  ";

					if(tokenCount == 0){

						type = atoi(token);

						//unrecognized type
						if(type > 4)
							break;
					}
					else{
						switch(type){
						case 0:		//global config
							if(tokenCount == 1){
								queueLength = atoi(token);
							}
							else if(tokenCount == 2){
								TTL = atoi(token);
							}
							break;
						case 1:		//router ID
							if(tokenCount == 1){
								parsedToken[0] = atoi(token);
							}
							else if(tokenCount==2){
								inet_aton(token, &tmpaddr);
								parsedToken[1] = tmpaddr.s_addr;
							}
							break;
						case 2:		//host ID
							if(tokenCount == 1){
								parsedToken[0] = atoi(token);
							}
							else if(tokenCount>=2){
								inet_aton(token, &tmpaddr);
								parsedToken[tokenCount-1] = tmpaddr.s_addr;
							}
							break;
						case 3:		//router-to-router
							switch(tokenCount){
							case 1:
							case 2:
							case 3:
								parsedToken[tokenCount-1] = atoi(token);
								break;
							case 4:
								delayList[parsedToken[0]][parsedToken[2]] = parsedToken[1];
								delayList[parsedToken[2]][parsedToken[0]] = atoi(token);
								break;
							}
							break;
						case 4:		//router-to-host

							string s;
							int idx;

							switch(tokenCount){
							case 1:
							case 2:
								parsedToken[tokenCount-1] = atoi(token);
								break;
							case 3:
								//split the IP address and subnet mask
								//store them as integers
								s = token;
								idx  = s.find('/');

								inet_aton(s.substr(0, idx).c_str(), &tmpaddr);
								parsedToken[2] = tmpaddr.s_addr;
								parsedToken[3] = maskGen(atoi(s.substr(idx+1, s.length()).c_str()));

								//cout<<parsedToken[2]<<" / "<<parsedToken[3]<<endl;

								break;
							case 4:
								parsedToken[4] = atoi(token);
								break;
							case 5:
								parsedToken[5] = atoi(token);
								delayList[parsedToken[0]][parsedToken[4]] = parsedToken[1];
								delayList[parsedToken[4]][parsedToken[0]] = parsedToken[5];
								break;
							}
							break;
						}

					}

					//catch back up to where we were
					//if(type==4 && tokenCount == 3)
					//	token = strtok(line, " ");
					//else
					token = strtok(NULL, " ");
					tokenCount++;
				}while(token!=NULL);
				cout<<endl;
			}
		}
		f.close();
	}
	//TODO map machine numbers to various data
	// example1: 3 1 100 2 110
	//delayList[1][2] = 100;
	//delayList[2][1] = 110;
	// example1: 4 1 1000 1.2.3.0/24 4 2000
	//delayList[1][4] = 1000;
	//delayList[4][1] = 2000;

	if (router == true) {
		return runRouter(&ip, queueLength); // TODO 10 is default Queue Length
	} else {
		return runHost(&ip, file, destIP, routerIP, TTL); // TODO 3 is default TTL
	}
}

int maskGen(int nbits){
	int mask;
	for(int i=0; i<32; i++){
		if(i<nbits)
			mask++;
		mask<<=1;
	}
	return mask;
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
