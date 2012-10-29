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
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo("127.0.0.1", "2012", &hints, &res);

	return 0;

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// connect!

	int result = connect(sockfd, res->ai_addr, res->ai_addrlen);

	printf("Connection Result: %i\n", result);

	return 0;
}
