/*
 * QTServer.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: cs3516
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cerrno>
#include "QTSession.h"

struct addrinfo hints;
struct addrinfo *res;
char input [500];

using namespace std;

int main(int argc, char* argv[]) {

	/*
	 -p PORT <port number>
	 -r RATE <number requests> <number seconds>
	 -u MAX_USERS <number of users>
	 -t TIME_OUT <number of seconds>
	 */

	/*if (argc < 5) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
	 std::cout
	 << "Usage is -p <port number> -r <rate> -u <max users> -t <time out>\n"; // Inform the user of how to use the program
	 std::cin.get();
	 //exit(0);
	 } else { // if we got enough parameters...
	 */
	char* portString = "2012";
	char* rateRequestsString = "3";
	char* rateTimeString = "60";
	char* usersString = "3";
	char* timeString = "80";

	std::cout << argv[0];

	for (int i = 1; i < argc; i++) {
		if ((i + 1) != argc) {

				//Port
			if (strcmp(argv[i], "-p") == 0) {
				portString = argv[i + 1];

				//Rate
			} else if (strcmp(argv[i], "-r") == 0) {
				rateRequestsString = argv[i + 1];
				i++;
				rateTimeString = argv[i + 1];

				//Max Users
			} else if (strcmp(argv[i], "-u") == 0) {
				usersString = argv[i + 1];

				//Time Out
			} else if (strcmp(argv[i], "-t") == 0) {
				timeString = argv[i + 1];

			}
		}
		std::cout << argv[i] << " ";
	}

	std::cout << "\nPort:  ";
	printf("%s\n", portString);
	std::cout << "Rate:  ";
	printf("%s requests in %s seconds\n", rateRequestsString, rateTimeString);
	std::cout << "Users: ";
	printf("%s\n", usersString);
	std::cout << "Time:  ";
	printf("%s\n", timeString);


	QTSession test(124);



	bool runServer = true;
	if(runServer){

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		int sockfd = -1;
		int status = -1;

		getaddrinfo("127.0.0.1", "2012", &hints, &res);

		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		printf("SOCKET returned: %i\n", sockfd);
		fflush(stdout);

		struct addrinfo *test = res;
		while (test != NULL) {
			if ((status = bind(sockfd, test->ai_addr, test->ai_addrlen)) == -1) {
				perror("Bind failed.");
				fflush(stdout);
				test = test->ai_next;
				continue;
			}

			printf("BIND returned: %d\n", status);
			fflush(stdout);
			//if(status != -1) break;
			break;
			//i=i->ai_next;
		}

		status = listen(sockfd, 5);
		printf("LISTEN returned: %i\n", status);
		fflush(stdout);

		struct sockaddr_storage addr;
		socklen_t sin_size;

		int acceptfd = -1;
		while (true) {
			sin_size = sizeof addr;
			acceptfd = accept(sockfd, (struct sockaddr *)&addr, &sin_size);
			if (acceptfd <= 0) {
				printf("%d\n", acceptfd);
				perror("Connection Attempted. Failed with error");
				fflush(stdout);
				continue;
			}
			break;
		}

		printf("ACCEPTED!\n");
		fflush(stdout);
		fflush(stdin);



		int bytesIn = 0;
		memset(&input, 0, sizeof input);

		while (true) {

			std::cout << "Testing for quit message... ";
			if(strcmp(input, "quit") == 0){
				break;
			}
			std::cout << "Not quitting" << endl ;

			std::cout << "Receiving... ";
			bytesIn = recv(acceptfd, &input, sizeof(input), 0);
			std::cout << "Received" << endl;

			if (bytesIn <= -1) {
				//cout<<"connection closed by client."<<endl<<"exiting..."<<endl;
				perror("RECV Error");
				break;
			} else {
				//cout << "RX: got \"" << input << "\", echoing back"<<endl;

				std::cout << "Adding null terminator... ";
				input[bytesIn] = '\0';
				std::cout << "Done." << endl;

				printf("server received: '%s'\n", input);


				char msg [550];
				msg[0] = '\0';

				//std::cout << "Appending Echo ";
				//std::cout << strcat(msg, "echo: ") << endl;
				std::cout << "Adding input... ";
				std::cout << strcat(msg, input) << endl;

				std::cout << "Sending response... ";
				send(acceptfd, msg, strlen(msg), 0);
				std::cout << "Done." << endl;
			}
		}
		close(sockfd);
	}

	//... some more code
	std::cin.get();
}
//}

