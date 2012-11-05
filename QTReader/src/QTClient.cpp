//============================================================================
// Author      :
// Name        : QTReader.cpp
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>


using namespace std;

int main(int argc, char* argv[]) {

	struct addrinfo hints;
	struct addrinfo *res;
	int sockfd;

	memset(&hints, 0, sizeof hints);
	memset(&res, 0, sizeof res);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo("127.0.0.1", "2012", &hints, &res);

	//return 0;

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// connect!

	int result = connect(sockfd, res->ai_addr, res->ai_addrlen);

	printf("Connection Result: %i\n", result);
	fflush(stdin);

	int bufSize = 512;
	char kbinput [bufSize];
	memset(&kbinput, 0, sizeof kbinput);
	char echoinput [bufSize];
	memset(&echoinput, 0, sizeof echoinput);
	int readSize;

	//snprintf(kbinput, 3,"%d", 10);

	while (true) {

		std::cout << "Checking for \"quit\"... ";
		if(strcmp(kbinput, "quit") == 0){
			break;
		}
		std::cout << "Done." << endl;

		std::cout << "Sending... ";
		send(sockfd, &kbinput, sizeof(kbinput), 0);
		std::cout << "Sent." << endl;

		std::cout << "Receiving... ";
		readSize = recv(sockfd, &echoinput, sizeof(echoinput), 0);
		std::cout << "Received." << endl;

		if (readSize == 0) {
			cout << "connection closed by server." << endl;
			break;
		} else {
			std::cout << "Bytes Received: ";
			std::cout << readSize;
			std::cout << " Data: ";
			cout << echoinput << endl;
			//sprintf(kbinput,"%l", random());
		}

	}
	close(sockfd);
	return 0;
}
