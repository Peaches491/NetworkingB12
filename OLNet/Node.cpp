//============================================================================
// Name        : OLRouter.cpp
// Author      : Daniel Miller and Kevin Janesch
// Description :
//============================================================================

#include "cs3516sock.h"
#include "Globals.h"
#include "Host.h"
#include "Packet.h"
#include "Router.h"
#include <arpa/inet.h>
#include <fstream>
#include <ifaddrs.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <list>

using namespace std;


typedef map<int, int> int_int_map;
map<int, int_int_map> delayList;
map<int, uint32_t> idToRealIP;
map<int, uint32_t> idToOverlayIP;
map<uint32_t, int> overlayIPToDeviceID;
map<int, list<int> > deviceList;
map<int, int> hostToRouter;

LPMTree* tree = new LPMTree();
PacketLogger* logger = new PacketLogger;

int deviceId = -1;

void getIP(in_addr* result);
//unsigned int maskGen(int nbits);

int main(int argc, char* argv[]) {

	bool router = false;
	LPMTree* tree = new LPMTree();

	in_addr* thisIP = (in_addr*)malloc(sizeof(in_addr));
	getIP(thisIP);
	cout << "Detected IP Address is: " << inet_ntoa(*thisIP) << endl;

	int queueLength = 15;
	int TTL = 15;
	if (1) {
		ifstream f;
		f.open("config.txt", ios::in | ios::binary);
//		cout << "opening 'config.txt'...." << endl;

		//seek to the end of the file
		f.seekg(0, ios::end);

		//file is open and not empty
		if (f.good() && f.tellg() > 0) {
			//hop back to the beginning
			f.seekg(0, ios::beg);

//			cout << "... opened. Parsing for config information..." << endl;

			while (!f.eof()) {
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
				if (!f.gcount())
					continue;

//				cout << "got tokens:  ";

				token = strtok(line, " ");

				do {
					//pull tokens out of it
//					cout << token << ",  ";

					if (tokenCount == 0) {

						type = atoi(token);

						//unrecognized type
						if (type > 4)
							break;
					} else {
						switch (type) {
						case 0:		//global config
							if (tokenCount == 1) {
								queueLength = atoi(token);
							} else if (tokenCount == 2) {
								TTL = atoi(token);
							}
							break;
						case 1:		//router ID
						case 2:		//host ID
							if (tokenCount == 1) {
								parsedToken[0] = atoi(token);
							} else if (tokenCount == 2) {
								inet_aton(token, &tmpaddr);
								parsedToken[1] = tmpaddr.s_addr;
								if (thisIP->s_addr == tmpaddr.s_addr){
									if(type==1) router = true;
									else router = false;
									deviceId = parsedToken[0];
								}
								idToRealIP[parsedToken[0]] = tmpaddr.s_addr;
							}
							else if (tokenCount == 3) {
								inet_aton(token, &tmpaddr);
								parsedToken[2] = tmpaddr.s_addr;
								idToOverlayIP[parsedToken[0]] = tmpaddr.s_addr;
								overlayIPToDeviceID[tmpaddr.s_addr] = parsedToken[0];
								tree->insert(parsedToken[0], ntohl(tmpaddr.s_addr), 32);
							}

							break;
						case 3:		//router-to-router
							switch (tokenCount) {
							case 1:
							case 2:
							case 3:
								parsedToken[tokenCount - 1] = atoi(token);
								break;
							case 4:
								int devA = parsedToken[0];
								int devB = parsedToken[2];
								delayList[devA][devB] =
										parsedToken[1];
								delayList[devB][devA] =
										atoi(token);

								deviceList[devA].push_back(devB);
								deviceList[devB].push_back(devA);
								break;
							}
							break;
						case 4:		//router-to-host

							string s;
							int idx;

							switch (tokenCount) {
							case 1:
							case 2:
								parsedToken[tokenCount - 1] = atoi(token);
								break;
							case 3:
								//split the IP address and subnet mask
								//store them as integers
								s = token;
								idx = s.find('/');

								inet_aton(s.substr(0, idx).c_str(), &tmpaddr);
								parsedToken[2] = tmpaddr.s_addr;
								parsedToken[3] = atoi( s.substr(idx + 1, s.length()).c_str() );

								//cout<<parsedToken[2]<<" / "<<parsedToken[3]<<endl;

								break;
							case 4:
								parsedToken[4] = atoi(token);
								break;
							case 5:
								parsedToken[5] = atoi(token);

								int devA = parsedToken[0];
								int devB = parsedToken[4];

								// Insert the data
								delayList[devA][devB] =
										parsedToken[1];
								delayList[devB][devA] =
										parsedToken[5];

								uint32_t lpmIP = parsedToken[2];
								uint32_t length = parsedToken[3];
								int queueNum = devA;
								tree->insert(queueNum, ntohl(lpmIP), length);

//								cout << "Adding " << devB << " to " << devA << endl;
								(deviceList[devA]).push_back(devB);
//								cout << deviceList[devA].back() << endl;
//								cout << "Adding " << devA << " to " << devB << endl;
								(deviceList[devB]).push_back(devA);
//								cout << deviceList[devB].back() << endl;

								hostToRouter[devB] = devA;

								idToOverlayIP[parsedToken[0]] = lpmIP ;

								break;
							}
							break;
						}

					}
					token = strtok(NULL, " ");
					tokenCount++;
				} while (token != NULL);
//				cout << endl;
			}
		}
		f.close();
	}

	cout << "---------------- Device List " << endl;
	map<int, uint32_t>::iterator it;
	for(it = idToOverlayIP.begin(); it != idToOverlayIP.end(); it++){
		in_addr addr;
		addr.s_addr = it->second;
		cout << it->first << " " << inet_ntoa(addr) << endl;
	}

	if (router == true) {
		return runRouter(thisIP, deviceId, queueLength, tree, logger, &delayList, &overlayIPToDeviceID, &idToRealIP, &deviceList);
	} else {
		return runHost(thisIP, deviceId, TTL, &idToRealIP, &idToOverlayIP, &hostToRouter, &delayList, &overlayIPToDeviceID);
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
			//result = (in_addr*)malloc(sizeof(struct in_addr));
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
