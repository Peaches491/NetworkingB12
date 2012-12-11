/*
 * FileSender.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: cs3516
 */

#include "FileSender.h"
#include "Packet.h"
#include "cs3516sock.h"
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory.h>

namespace std {

int getFileSize(FILE* fd);
void* senderFunc(void* ptr);

FileSender::FileSender(data d) {
	this->sock = d.sock;
	this->id = d.id;
	this->file = d.file;
	this->hostOverlayIPString = d.hostOverlayIPString;
	this->destRealIPString = d.destRealIPString;
	this->routerRealIPString = d.routerRealIPString;
	this->ttl = d.ttl;
	this->sourcePort = d.sourcePort;
	this->destPort = d.destPort;
	this->deviceID = d.deviceID;
	this->routerID = d.routerID;
	this->delay = d.delay;
	this->running = false;
	this->thread = new pthread_t;
}

FileSender::~FileSender() {
	// TODO Auto-generated destructor stub
}

pthread_t* FileSender::send() {
	cout << "Starting thread... ";
	this->running = true;
	if (pthread_create(this->thread, NULL, (&(senderFunc)), (void*) this)
			== 0) {
		cout << "Done." << endl;
		return this->thread;
	} else {
		cout << "Whoops!" << endl;
		return NULL;
	}
}

void* senderFunc(void* ptr) {
	FileSender* sndr = (FileSender*) ptr;
	cout << "Opening file to be sent... ";
	string fileString = string(sndr->file);
	FILE* fd = fopen(sndr->file, "r");
	cout << fileString << endl;
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

	createAndSendPacket(sndr->sock, sndr->id, (char*) &size, sizeof(uint32_t),
			sndr->hostOverlayIPString, sndr->destRealIPString,
			sndr->routerRealIPString, sndr->ttl, sndr->sourcePort,
			sndr->destPort);
	while (sent < size) {
		int bytesIn = fread((void*) readBuf, 1, MAX_PAYLOAD, fd);
		dataBuf = (char*) realloc(dataBuf, bytesIn);
		memcpy(dataBuf, readBuf, bytesIn);
		sent += createAndSendPacket(sndr->sock, sndr->id, dataBuf, bytesIn,
				sndr->hostOverlayIPString, sndr->destRealIPString,
				sndr->routerRealIPString, sndr->ttl, sndr->sourcePort,
				sndr->destPort);
		usleep(sndr->delay * 1000);
	}
	fclose(fd);
	return 0;
}

int getFileSize(FILE* fd) {
	int size = -1;
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	return size;
}
} /* namespace std */
