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
#include <fstream>
#include <unistd.h>

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

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	printf("Socket FD: %i\n", sockfd);
	// connect!

	int result = connect(sockfd, res->ai_addr, res->ai_addrlen);

	printf("Connection Result: %i\n", result);

	if (result < 0) {
		cout << "no connection was established. exiting." << endl;
		return 1;
	}

	fflush(stdin);

	string kbinput;
	int readSize;

	ifstream image;
	unsigned int imagesize;
	string data;
	unsigned int datasize = 0;
	unsigned int response = 0;

	while (true) {

		cout << "enter the path of an image: ";
		cin >> kbinput;

		FILE *stream = popen((std::string("echo ") + kbinput.c_str()).c_str(),
				"r");

		char buf[512];
		fgets(buf, sizeof(buf), stream);

		//cout << buf << endl;

		string path(buf);

		path = path.substr(0, path.size() - 1);
		//cout << path << endl;

		//std::cout << "Checking for \"quit\"... ";
		if (path.compare("quit\n") == 0) {
			break;
		}
		//std::cout << "Done." << endl;

		//if(valid path){
		//open the image
		image.open(path.c_str(), ios::in | ios::binary);

		if (!image.is_open()) {
			cout << "error opening image at " << path << endl;
			continue;
			//break;
		}

		image.seekg(0, ios::beg);
		int beginning = (int) image.tellg();
		//determine the image's size
		image.seekg(0, ios::end);
		int end = (int) image.tellg(); //-imagesize;

		imagesize = end - beginning;

		cout << "image size: " << imagesize << " B" << endl;

		//seek to the beginning of the image
		image.seekg(0, ios::beg);

		//load the image into memory
		char* contents = new char[imagesize];
		image.read(contents, imagesize);
		image.close();

		//image.
		//send the image size and data
		send(sockfd, &imagesize, 4, 0);
		cout << "sent image size." << endl; //: "<<imagesize<<"B"<<endl;

		//wait for an OK from the server
		recv(sockfd, &response, sizeof(int), 0);
		cout << "got response: size " << (response == 0 ? "OK" : "NOT OK")
				<< endl;
		if (response != 0)
			continue;

		//send image

		unsigned int dataSent = 0;
		while (dataSent < imagesize) {
			//std::cout << "Byte " << dataSent << endl;
			send(sockfd, &(contents[dataSent]), sizeof(contents[0]), 0);
			dataSent++;
			if (dataSent >= imagesize)
				break;
			//std::cout << "Sent " << bytesIn << " bytes." << endl;
		}

		cout << "sent image to server. waiting for result..." << endl;

		//free dat RAM
		delete[] contents;

		//recv(sockfd, &datasize, 4, 0);

		//wait for server response code
		recv(sockfd, &response, sizeof(int), 0);
		cout << "server response: "
				<< (response == 0 ?
						"QR CODE OK" :
						(response == 1 ?
								"FAILED" :
								(response == 2 ?
										"TIMED OUT" :
										(response == 3 ?
												"RATE EXCEEDED" : "MISC ERROR"))))
				<< endl;
		if (response == 2) {
			break;
		} else if (response > 0){
			continue;
		} else {
			recv(sockfd, &datasize, sizeof(int), 0);

			readSize = 0;
			char tempdata[datasize + 128];
			memset(tempdata, 0, sizeof tempdata);

			do {
				readSize += recv(sockfd, &tempdata, sizeof tempdata, 0);

				//tempdata = "";
			} while (readSize < datasize);

			data = tempdata;

			cout << "QR data: " << endl << data << "" << endl;

			//account for the possibility of data getting smeared across packets

			if (readSize == 0) {
				cout << "connection closed by server." << endl;
				break;
			}
		}
		//1: string
		//2: image
		//	4 bytes for size
		//	n bytes for image
		//  wait for '1' back
		//	wait for data
	}
	close(sockfd);
	return 0;
}
