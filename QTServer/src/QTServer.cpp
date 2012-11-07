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
// 1 B
//#define IMAGE_SIZE_LIMIT (1)
// 1 KB
//#define IMAGE_SIZE_LIMIT (1024)
// 1 MB
//#define IMAGE_SIZE_LIMIT (1049000)
// 15 MB
#define IMAGE_SIZE_LIMIT (15735000)
// 500 MB
//#define IMAGE_SIZE_LIMIT (524288000)

using namespace std;

// Global Variables
static const char* imgDir = "images/";
char inputBuffer[INPUT_BUFFER_SIZE]; //Initial input buffer
struct addrinfo hints;
struct addrinfo *res;

// Function Declarations
int badRECVCheck(int bytesIn);
string readQRCode(const char* fpath, struct sockaddr* addr);
int serviceClient(struct sockaddr_storage addr, int acceptfd);

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

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	int sockfd = -1;
	int status = -1;

	getaddrinfo("127.0.0.1", portString, &hints, &res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	log("Socket created.");

	int yes = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	log("Enabled address reuse.");

	struct timeval timeout;
	timeout.tv_sec = atoi(timeString);
	timeout.tv_usec = 0;

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))
			< 0) {
		perror("setsockopt");
		exit(1);
	}
	log((std::string("Enabled socket timeout ") + timeString).c_str());

	struct addrinfo *test = res;
	while (test != NULL) {
		if ((status = bind(sockfd, test->ai_addr, test->ai_addrlen)) == -1) {
			perror("Bind failed");
			fflush(stdout);
			test = test->ai_next;
			continue;
		}

		break;
	}
	log((std::string("Socket bound to port ") + portString + ".").c_str());

	status = listen(sockfd, 5);

	int numClients = 0;

	bool runServer = true;
	while (runServer) {

		log((std::string("Listening on port ") + portString + ".").c_str());

		struct sockaddr_storage addr;
		socklen_t sin_size;

		int acceptfd = -1;
		while (true) {
			memset(&addr, 0, sizeof(addr));
			memset(&sin_size, 0, sizeof(sin_size));
			sin_size = sizeof addr;
			acceptfd = accept(sockfd, (struct sockaddr *) &addr, &sin_size);
			if (acceptfd <= 0) {
				numClients++;
				continue;
			}
			break;
		}

		ostringstream ss;
		ss << numClients;
		log((struct sockaddr *) &addr,
				(std::string("Accepted client connection #") + ss.str().c_str()
						+ ".").c_str());

		pid_t forkResult = fork();

		if (forkResult != 0) {
			serviceClient(addr, acceptfd);
		} else {
			// Add PID to running queue od MAX USERS size
			numClients++;
		}

	}
	close(sockfd);
	//... some more code
	std::cin.get();
}

string readQRCode(const char* fpath, struct sockaddr* addr) {
	FILE *fpipe;
	int linelength = 300;
	char* line = new char[linelength];

	string command = string(ZXING_PATH) + fpath;
	string output;
	string interpretedCode;

	int8_t storeNextLine = 0;

	// this'll call the ZXing command
	// but it'll also allow us to crawl the output
	// for the delicious interpreted QR code
	if (!(fpipe = (FILE*) popen(command.c_str(), "r"))) {
		perror("Problem executing QR code interpreter");
		return "";
	}

	//read in ALL the lines!
	while (fgets(line, linelength, fpipe)) {
		if (storeNextLine == 1) {
			interpretedCode = string(line);
			interpretedCode.resize(interpretedCode.length() - 1);
			storeNextLine = -1;	// disable any further writes to the string
		}
		//TODO: Probably wanna add this to the log. Maybe just swap this line for that?
		output += string(line);

		//will flip to one when not storing a line and the current line marks the data
		if (storeNextLine >= 0)
			storeNextLine = (string(line).find(PARSED_DATA_MARKER) == 0);

		//line = "";
	}

	//close the stream/"file"/command
	pclose(fpipe);

	//no interpreted code was found.
	if (output == "") {
		log(addr, (std::string("Got no data from ZXing.")).c_str());
		return string("");
	}

	log(addr,
			(std::string("String returned from ZXing \"") + interpretedCode
					+ "\"").c_str());

	//cleanup
	delete[] line;
	//if(&fpipe) delete fpipe;
	//delete &output;
	//delete &command;

	return interpretedCode;
}

int badRECVCheck(int bytesIn) {
	if (bytesIn <= 0) {
		//std::cout << "No bytes received. Socket must be closed." << endl;
		return 1;
	} else {
		return 0;
	}
}

int serviceClient(sockaddr_storage addr, int acceptfd) {
	bool runSession = true;
	while (runSession) {
		int bytesIn = 0;
		memset(&inputBuffer, 0, sizeof inputBuffer);

		if (strcmp(inputBuffer, "quit") == 0) {
			break;
		}

		log((struct sockaddr *) &addr, "Waiting for data.");

		int messageSize = 0;
		int hasTimedOut = 0;
		bytesIn = recv(acceptfd, &messageSize, sizeof(int), 0);
		if (badRECVCheck(bytesIn)) {
			log((struct sockaddr *) &addr, "Timed Out.");
			hasTimedOut = 1;
		}

		// Start assembling the image file.
		// Set flag to IMAGE

		// Take in the image size
		int imageSize = messageSize;
		int imageTooBig = 0;

		if (imageSize == -1) {
			exit(0);
		} else if (imageSize > IMAGE_SIZE_LIMIT) {
			log((struct sockaddr *) &addr,
					(std::string("Client offered image too large.")).c_str());
			imageTooBig = 1;
		}

		if (hasTimedOut)
			imageTooBig = 2;
		send(acceptfd, &imageTooBig, sizeof(imageTooBig), 0);

		if (hasTimedOut) {
			break;
		}
		if (imageTooBig) {
			continue;
		}


		ofstream file;

		ostringstream ss;
		if (mkdir(imgDir, S_IRWXU | S_IRWXG | S_IRWXO) == 0) {
			log((struct sockaddr *) &addr,
					(std::string("Creating directory ") + imgDir).c_str());
		}
		ss << getpid();
		std::string filename = (std::string(imgDir) + (char*) ss.str().c_str()
				+ std::string(".png"));
		file.open(filename.c_str(), ios::app);
		log((struct sockaddr *) &addr,
				(std::string("Writing image file ") + filename).c_str());

		int size = 0;
		while (size < imageSize) {
			bytesIn = recv(acceptfd, &inputBuffer, INPUT_BUFFER_SIZE, 0);
			if (badRECVCheck(bytesIn)) {
				log((struct sockaddr *) &addr, "Timed Out 2.");
				break;
			}

			for (int i = 0; i < bytesIn; i++) {

				file << inputBuffer[i];
				size++;
				if (size >= imageSize)
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

		// Parse the QR Code and transmit
		int code = 0;
		int resultlen = 0;

		log((struct sockaddr *) &addr,
				(std::string("Reading QR Code stored at ") + filename).c_str());
		string result = readQRCode(filename.c_str(), (struct sockaddr*) &addr);

		std::string resultString(result);

		if (resultString.compare("") == 0) {
			code = 1;
			result = "NULL";
		}
		log((struct sockaddr *) &addr,
				(std::string("QR Code result was \"") + result + "\"").c_str());
		resultlen = resultString.length();

		// Transmit!
		send(acceptfd, &code, 4, 0);

		send(acceptfd, &resultlen, 4, 0);

		send(acceptfd, result.c_str(), strlen(result.c_str()), 0);
	}
	return 0;
}
