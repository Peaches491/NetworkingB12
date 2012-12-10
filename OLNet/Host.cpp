/*
 * Host.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <cerrno>
#include <fstream>
#include <signal.h>
#include <sstream>

#include "Host.h"
#include "Packet.h"
//#include "Globals.h"
#include "cs3516sock.h"

using namespace std;

int id = 0;
FILE* rFile;
int hostSock = -1;

static void terminate(int arg) {
	cout << endl << "Terminating host... " << endl;

	cout << "Closing receive file... ";
	if (rFile != NULL)
		fclose(rFile);
	cout << "Done." << endl;

	cout << "Closing socket... ";
	close(hostSock);
	cout << "Done." << endl;

	exit(1);
}

int runHost(in_addr* _ip, int deviceID, int ttl, map<int, uint32_t>* idToRealIP,
		map<int, uint32_t>* idToOverlayIP, map<int, int>* hostToRouter,
		map<int, map<int, int> >* delayList) {
	char* file = "send_body";
	in_addr ip;
	memcpy(&ip, _ip, sizeof(in_addr));

	////////////////////////////////
	// DO NOT TOUCH THESE MEMCPYs //
	////////////////////////////////
	int routerID = (*hostToRouter)[deviceID];

	in_addr routerBinIP;
	routerBinIP.s_addr = (*idToRealIP)[routerID];
	char* routerRealIPString = (char*) malloc(16);
	memcpy(routerRealIPString, inet_ntoa(routerBinIP), 16);
	//temp3[15] = '\0';

	char* routerOverlayIPString = (char*) malloc(16);
	in_addr tmp;
	tmp.s_addr = (*idToOverlayIP)[routerID];
	memcpy(routerOverlayIPString, inet_ntoa(tmp), 16);
	//routerOverlayIPString[15] = '\0';

	char* hostOverlayIPString = (char*) malloc(16);
	tmp.s_addr = (*idToOverlayIP)[deviceID];
	memcpy(hostOverlayIPString, inet_ntoa(tmp), 16);
	//hostOverlayIPString[15] = '\0';

	char* hostRealIPString = (char*) malloc(16);
	memcpy(hostRealIPString, inet_ntoa(ip), 16);
	//temp1[15] = '\0';
	////////////////////////////////
	// DO NOT TOUCH THESE MEMCPYs //
	////////////////////////////////

	cout << "---------- Host Mode Started " << endl;
	cout << "File:       " << file << endl;
	cout << "Host ID:    " << deviceID << endl;
	cout << "Host IP:    " << hostRealIPString << endl;
	cout << "Overlay IP: " << hostOverlayIPString << endl;
	cout << "Router ID:  " << routerID << endl;
	cout << "Router IP:  " << routerRealIPString << endl;
	cout << "Overlay IP: " << routerOverlayIPString << endl;

	bool sending = true;

	signal(SIGINT, terminate);
	signal(SIGABRT, terminate);
	signal(SIGTERM, terminate);

	hostSock = create_cs3516_socket();
	//cout << hostSock << endl;

	ifstream fd("send_config.txt");
	if (!fd.good())
		sending = false;
	char* configBuf = new char[100];
	fd.getline(configBuf, 100);

	string line(configBuf);
	string param;
	string lineStr(line);
	stringstream lineStream(line);

	lineStream >> param;
	string destIP(param.c_str());
	char* destRealIPString = (char*) malloc(16);
	memcpy(destRealIPString, destIP.c_str(), 16);

	lineStream >> param;
	int sourcePort = atoi(param.c_str());

	lineStream >> param;
	int destPort = atoi(param.c_str());
	fd.close();

	if (sending) {
		cout << "Opening file to be sent... ";
		string fileString = string(file);
		FILE* fd = fopen(file, "r");
		if (fd == NULL) {
			cout << endl
					<< "Could NOT open the file to be sent 'send_body'. Terminating..."
					<< endl;
			abort();
		}

		char* readBuf = (char*) malloc(MAX_PAYLOAD);
		char* dataBuf = (char*) malloc(MAX_PAYLOAD);

		int size = getFileSize(fd);
		int sent = 0;
		cout << "Done." << endl;
		cout << "Filesize is: " << size << endl;

		createAndSendPacket(hostSock, &id, (char*) &size, sizeof(uint32_t),
				hostOverlayIPString, destRealIPString, routerRealIPString, ttl, sourcePort, destPort);
		while (sent < size) {
			int bytesIn = fread((void*) readBuf, 1, MAX_PAYLOAD, fd);
			dataBuf = (char*) realloc(dataBuf, bytesIn);
			memcpy(dataBuf, readBuf, bytesIn);
			sent += createAndSendPacket(hostSock, &id, dataBuf, bytesIn, hostOverlayIPString,
					destRealIPString, routerRealIPString, ttl, sourcePort, destPort);
			usleep((*delayList)[deviceID][routerID]*1000);
		}
	} else {
		cout << "No file specified. Waiting for data." << endl;
	}

	cout << "Opening receive file... ";
	rFile = fopen("received", "w");
	if (rFile == NULL) {
		cout << "Receive file could not be opened. Exiting." << endl;
		abort();
	}
	cout << "Done." << endl;

	char* buf = (char*) malloc(MAX_PACKET_SIZE);
	cs3516_recv(hostSock, buf, MAX_PACKET_SIZE);
	int packetCount = 0;
	while (1) {

		buf = (char*) realloc(buf, MAX_PACKET_SIZE);
		memset(buf, 0, MAX_PACKET_SIZE);

		int recv = cs3516_recv(hostSock, buf, MAX_PACKET_SIZE);
		packetCount++;

		cout << "Recv: " << recv << " bytes" << endl;

		buf = (char*) realloc(buf, recv);

		packethdr* p = ((packethdr*) buf);
		char* data = (char*) (buf + sizeof(packethdr));

		//printPacket(p);
		cout << "Recieved " << packetCount << " packets." << endl;

		printPacket(p);

		usleep(50);

		for (int i = 0; i < p->udp_header.len; i++) {
			fputc(data[i], rFile);
		}

		//char* data = buf + sizeof(packethdr);
		//cout << data << endl;
	}
	return 0;
}

int getFileSize(FILE* fd) {
	int size = -1;
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	return size;
}
