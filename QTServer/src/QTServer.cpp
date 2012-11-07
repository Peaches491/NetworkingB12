/*
 * QTServer.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: cs3516
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cerrno>
#include <algorithm>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include "QTServer.h"
#include "QTSession.h"
#include "Logger.h"

#define INPUT_BUFFER_SIZE (1024)

using namespace std;

// Global Variables
static const char* imgDir = "images/";
char inputBuffer[INPUT_BUFFER_SIZE]; //Initial input buffer
struct addrinfo hints;
struct addrinfo *res;

typedef struct _message {
	messageCode code;
	int size;
	void* data;
} message;

// Function Declarations
int checkRECV(int bytesIn, void* data);

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

	std: cout << "" << endl;

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

	//std::cout << "\nPort:  ";
	//printf("%s\n", portString);
	//std::cout << "Rate:  ";
	//printf("%s requests in %s seconds\n", rateRequestsString, rateTimeString);
	//std::cout << "Users: ";
	//printf("%s\n", usersString);
	//std::cout << "Time:  ";
	//printf("%s\n", timeString);

	bool runServer = true;
	if (runServer) {

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		int sockfd = -1;
		int status = -1;

		getaddrinfo("127.0.0.1", portString, &hints, &res);

		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		log("Socket created.");

		int yes = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
			perror("setsockopt");
			exit(1);
		}
		log("Enabled address reuse.");

		struct addrinfo *test = res;
		while (test != NULL) {
			if ((status = bind(sockfd, test->ai_addr, test->ai_addrlen))
					== -1) {
				perror("Bind failed");
				fflush(stdout);
				test = test->ai_next;
				continue;
			}

			//printf("BIND returned: %d\n", status);
			//fflush(stdout);
			//if(status != -1) break;
			break;
			//i=i->ai_next;
		}
		log((std::string("Socket bound to port ") + portString + ".").c_str());

		status = listen(sockfd, 5);
		log((std::string("Listening on port ") + portString + ".").c_str());
		//printf("LISTEN returned: %i\n", status);
		//fflush(stdout);

		struct sockaddr_storage addr;
		socklen_t sin_size;

		int acceptfd = -1;
		while (true) {
			sin_size = sizeof addr;
			acceptfd = accept(sockfd, (struct sockaddr *) &addr, &sin_size);
			if (acceptfd <= 0) {
				//printf("%d\n", acceptfd);
				//perror("Connection Attempted. Failed with error");
				//fflush(stdout);
				continue;
			}
			break;
		}

		log((struct sockaddr *) &addr, "Accepted client connection.");

		int bytesIn = 0;
		memset(&inputBuffer, 0, sizeof inputBuffer);

		while (true) {
			if (strcmp(inputBuffer, "quit") == 0) {
				break;
			}

			log((struct sockaddr *) &addr, "Waiting for data.");

			int messageSize = 0;
			bytesIn = recv(acceptfd, &messageSize, sizeof(int), 0);


			// Start assembling the image file.
			// Set flag to IMAGE
			message msg;
			msg.code = IMAGE;

			// Take in the image size
			msg.size = -1;
			//std::cout << "Reading in size of data... ";
			msg.size = messageSize;
			//std::cout << "Done." << endl;
			//std::cout << "Size was " << msg.size << endl;

			if (msg.size == -1) {
				exit(0);
			}

			int text = 0;
			send(acceptfd, &text, sizeof(text), 0);

			ofstream file;

			ostringstream ss;
			if(mkdir(imgDir, S_IRWXU | S_IRWXG | S_IRWXO) == 0){
				log((struct sockaddr *) &addr, (std::string("Creating directory ") + imgDir).c_str());
			}
			ss << getpid();
			std::string filename = (std::string(imgDir) + (char*) ss.str().c_str() + std::string(".png"));
			file.open(filename.c_str(), ios::app);
			log((struct sockaddr *) &addr, (std::string("Writing image file ") + filename).c_str());

			int size = 0;
			while (size < msg.size) {
				bytesIn = recv(acceptfd, &inputBuffer, INPUT_BUFFER_SIZE, 0);

				for (int i = 0; i < bytesIn; i++) {
					size++;
					if (size >= msg.size)
						break;
				}

				//std::cout << "Added " << bytesIn << " bytes." << endl;
			}
			stringstream strstr;
			strstr << "Image file " << size << " Bytes";
			log((struct sockaddr *) &addr, strstr.str().c_str());

			log((struct sockaddr *) &addr,
					(std::string("Closed image file ") + filename).c_str());
			file.close();
		}
		close(sockfd);
	}

	//... some more code
	std::cin.get();
}
//}

string interpretCode() {
	FILE *stream = popen((string("echo ")).c_str(), "r");

	char buf[512];
	fgets(buf, sizeof(buf), stream);

	cout << buf << endl;

	return string(buf);
}

int checkRECV(int bytesIn, void* data) {
	if (bytesIn <= -1) {
		//cout<<"connection closed by client."<<endl<<"exiting..."<<endl;
		perror("RECV Error");
		return 0;
	} else if (bytesIn <= 0) {
		std::cout << "No bytes received. socket must be broken." << endl;
		return 0;
	} else {
		return 1;
	}
}

