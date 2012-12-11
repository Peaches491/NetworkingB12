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
#include "FileSender.h"

using namespace std;

int id = 0;
FILE* rFile;
ofstream stats;
map<int, fileData>files;
int hostSock = -1;

void printFileStats(fileData d);

static void terminate(int arg) {

	map<int, fileData>::iterator it;
	for (it = files.begin(); it != files.end(); ++it) {
		printFileStats(it->second);
	}


	cout << endl << "Terminating host... " << endl;

	cout << "Closing receive file... ";
	if (rFile != NULL)
		fclose(rFile);
	cout << "Done." << endl;

	cout << "Closing receive file... ";
	stats.close();
	cout << "Done." << endl;

	cout << "Closing socket... ";
	close(hostSock);
	cout << "Done." << endl;

	exit(1);
}

int runHost(in_addr* _ip, int deviceID, int ttl, map<int, uint32_t>* idToRealIP,
		map<int, uint32_t>* idToOverlayIP, map<int, int>* hostToRouter,
		map<int, map<int, int> >* delayList, map<uint32_t, int>* overlayIPToDeviceID) {
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
		//(char* file, int delay, char* hostOverlayIPString, char* destRealIPString, char* routerRealIPString, int ttl, int sourcePort, int destPort, int deviceID, int routerID)
		data d;
		d.delay = (*delayList)[deviceID][routerID];
		d.destPort = destPort;
		d.destRealIPString = destRealIPString;
		d.deviceID = deviceID;
		d.file = file;
		d.hostOverlayIPString = hostOverlayIPString;
		d.id = &id;
		d.routerID = routerID;
		d.routerRealIPString = routerRealIPString;
		d.sock = hostSock;
		d.sourcePort = sourcePort;
		d.ttl = ttl;

		FileSender* sender = new FileSender(d);
		sender->send();
	} else {
		cout << "No file specified. Waiting for data." << endl;
	}

	cout << "Opening receive file... ";
	rFile = fopen("received", "a+");
	if (rFile == NULL) {
		cout << "Receive file could not be opened. Exiting." << endl;
		abort();
	}
	cout << "Done." << endl;

	cout << "Opening receive_stats file... ";
	stats.open("received_stats.txt", ifstream::out | ifstream::app);
	if (!stats.good()) {
		cout << "Receive Stats file could not be opened. Exiting." << endl;
		abort();
	}
	cout << "Done." << endl;


	while (1) {
		char* buf = (char*) malloc(MAX_PACKET_SIZE);
		//cs3516_recv(hostSock, buf, MAX_PACKET_SIZE);
		int packetCount = 0;
		int totalData = 0;
		int totalDataSansHeaders = 0;

		memset(buf, 0, MAX_PACKET_SIZE);

		int recv = cs3516_recv(hostSock, buf, MAX_PACKET_SIZE);
		buf = (char*) realloc(buf, recv);

		packethdr* p = ((packethdr*) buf);
		char* data = (char*) (buf + sizeof(packethdr));

		int sourceID = (*overlayIPToDeviceID)[p->ip_header.ip_src.s_addr];

		if(p->ip_header.ip_id == 0){
			files[sourceID].deviceID = sourceID;
			int fileSize = *((int*)data);
			files[sourceID].filesize = fileSize;
			files[sourceID].packetCount = (fileSize/MAX_PAYLOAD);
			if(fileSize%MAX_PAYLOAD != 0) files[sourceID].packetCount++;
			files[sourceID].packets = 0;
			files[sourceID].totalData = 0;
			files[sourceID].totalDataSansHeaders = 0;

			for(int i = 1; i <= files[sourceID].packetCount; i++){
				files[sourceID].missingPackets.push_back(i);
			}

			stringstream ss;
			ss << sourceID;
			files[sourceID].filename = string("receive_") + ss.str();
			files[sourceID].file = fopen(files[sourceID].filename.c_str(), "w+");
			fclose(files[sourceID].file);

			cout << endl;
			printf("Receiving file of size: %d\n", fileSize);
			continue;
		}

		map<int, fileData>::iterator it = files.find(sourceID);
		fileData fData;
		if(it != files.end()) {

			files[sourceID].packets++;

			files[sourceID].missingPackets.remove(p->ip_header.ip_id);

			//cout << "Recv: " << recv << " bytes" << endl;
			files[sourceID].totalData += recv;
			files[sourceID].totalDataSansHeaders += (recv - sizeof(packethdr));

			buf = (char*) realloc(buf, recv);

			cout << "Received packet #" << p->ip_header.ip_id << " " << recv << " bytes from device " << sourceID << endl;
			cout << files[sourceID].packets << " of " << files[sourceID].packetCount << " packets totalling " << files[sourceID].totalData << " bytes (" << files[sourceID].totalDataSansHeaders << " payload)"<< endl;

			stats << inet_ntoa(p->ip_header.ip_src);
			stats << " " << inet_ntoa(p->ip_header.ip_dst);
			stats << " " << p->udp_header.source;
			stats << " " << p->udp_header.dest << endl;

			files[sourceID].file = fopen(files[sourceID].filename.c_str(), "a+");
			for (int i = 0; i < p->udp_header.len; i++) {
				fputc(data[i], files[sourceID].file);
			}
			fclose(files[sourceID].file);

			if(files[sourceID].totalDataSansHeaders == files[sourceID].filesize){
				cout << "File " << sourceID << " Complete." << endl;
				printFileStats(files[sourceID]);
			}
		}
	}

	return 0;
}

void printFileStats(fileData d){

	cout << endl << "File ----------------------" << endl;
	cout << "Filename:         " << d.filename << endl;
	cout << "Source Device:    " << d.deviceID << endl;
	cout << "Filesize:         " << d.filesize << endl;
	cout << "Expected Packets: " << d.packets << endl;
	cout << "Recieved Data:    " << d.totalDataSansHeaders << endl;
	cout << "Missing Packets:  ";

	if(d.missingPackets.empty()) cout << "None.";
	else{
		list<int>::iterator it;
		for (it = d.missingPackets.begin(); it!=d.missingPackets.end(); ++it) {
			cout << *it << " ";
		}
	}
	cout << endl;
}

int getFileSize(FILE* fd) {
	int size = -1;
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	return size;
}
