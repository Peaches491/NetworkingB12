/*
 * Log.cpp
 *
 *  Created on: Dec 8, 2012
 *      Author: cs3516
 */

#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <strings.h>

#include "PacketLogger.h"
#include "Packet.h"

namespace std {

void PacketLogger::initLogger() {
	this->logF.open("ROUTER_control.txt", ios::out);
	if (logF == NULL) {
		cout << "Log file could not be opened. Exiting." << endl;
		abort();
	}
}

void PacketLogger::closeLogger() {
	if (logF != NULL)
		logF.close();
}

void PacketLogger::logPacket(packethdr* p, LogType type) {
	in_addr adr { 0 };
	logPacket(p, type, adr);
}

void PacketLogger::logPacket(packethdr* p, LogType type, in_addr nextHop) {

	char* delim = " ";
	char* str = (char*) malloc(100);
	char* next = (char*) malloc(16);
	memcpy(next, inet_ntoa(nextHop), 16);
	char* src = (char*) malloc(16);
	memcpy(src, inet_ntoa(p->ip_header.ip_src), 16);
	char* dst = (char*) malloc(16);
	memcpy(dst, inet_ntoa(p->ip_header.ip_dst), 16);
	bzero(str, 100);

	if (type == SENT_OKAY) {
		sprintf(str, "%ld%s%s%s%s%s%d%s%s%s%s", (long int) time(NULL), delim,
				src, delim,
				dst, delim, p->ip_header.ip_id,
				delim, (typeString[type]).c_str(), delim, next);
	} else {
		sprintf(str, "%ld%s%s%s%s%s%d%s%s", (long int) time(NULL), delim,
				src, delim,
				dst, delim, p->ip_header.ip_id,
				delim, (typeString[type]).c_str());
	}

	cout << str << endl;
	pthread_mutex_lock(&fileLock);
	logF << str << endl;
	pthread_mutex_unlock(&fileLock);
	delete str;
}

}
