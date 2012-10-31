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

	//return 0;

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// connect!

	int result = connect(sockfd, res->ai_addr, res->ai_addrlen);

	printf("Connection Result: %i\n", result);
	fflush(stdin);

	char* kbinput = "benderisgreat";
	char* echoinput = "                                             \0";
	int readsize;

	//snprintf(kbinput, 3,"%d", 10);

	while(strcmp(kbinput, "quit") != 0){
		send(sockfd, &kbinput, sizeof(kbinput), 0);
		readsize = recv(sockfd, &echoinput, 15, 0);

		if(readsize==0){
			cout<<"connection closed by server."<<endl;
			break;
		}
		else{
			cout<<echoinput<<endl;
			//sprintf(kbinput,"%l", random());
		}

	}
	close(sockfd);
	return 0;
}
