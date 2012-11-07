/*
 * Logger.cpp
 *
 *  Created on: Nov 6, 2012
 *      Author: cs3516
 *

 YYYY-MM-DD HH:mm:ss xxx.xxx.xxx.xxx
 - Valid and Invalid QR Code interactions
 - start-ups
 - connections
 - disconnections from users
 - events as described by the RATE/MAX_USERS instructions
 - any other information regarding user behavior or system events.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include "Logger.h"

using namespace std;

std::string getTimestamp();
std::string getAddr(struct sockaddr *sock);

void log(const char* mesg){
	std::cout << getTimestamp() << " " << mesg << endl;
}

void log(struct sockaddr* sock, const char* mesg){
	std::cout << getTimestamp() << " " << getAddr(sock) << " " << mesg << endl;
}

std::string getTimestamp() {

	time_t t;
	struct tm * timeinfo;
	char buffer[80];

	time(&t);
	timeinfo = localtime(&t);

	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
	string s(buffer);

	return s;
}

std::string getAddr(struct sockaddr *sock) {
	char str [INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(((struct sockaddr_in*)sock)->sin_addr), str, INET_ADDRSTRLEN);
	std::string s(str);
	return (s);
}


