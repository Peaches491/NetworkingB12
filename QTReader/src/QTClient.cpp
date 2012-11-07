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

	//return 0;

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// connect!

	int result = connect(sockfd, res->ai_addr, res->ai_addrlen);

	printf("Connection Result: %i\n", result);
	fflush(stdin);

	//int bufSize = 512;
	string kbinput;
	//memset(&kbinput, 0, sizeof kbinput);
	//char echoinput [bufSize];
	//memset(&echoinput, 0, sizeof echoinput);
	int readSize;

	ifstream image;
	unsigned int imagesize;
	string data;
	string tempdata;
	unsigned int datasize = 0;
	unsigned int response = 0;
	//snprintf(kbinput, 3,"%d", 10);

	while (true) {

		cout << "enter the path of an image: ";
		cin >> kbinput;


		FILE *stream = popen((std::string("echo ") + kbinput.c_str()).c_str(), "r");

		char buf [512];
		fgets(buf, sizeof(buf), stream);

		//cout << buf << endl;

		string path(buf);

		path = path.substr(0, path.size()-1);
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
			cout << "error opening image " << path << endl;

			continue;
		}

		image.seekg(0, ios::beg);
		int beginning = (int) image.tellg();
		//determine the image's size
		image.seekg(0, ios::end);
		int end = (int) image.tellg();		//-imagesize;

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
		cout << "sent image size." << endl;		//: "<<imagesize<<"B"<<endl;

		//wait for an OK from the server
		recv(sockfd, &response, 1, 0);
		cout << "got response: size" << (response == 0 ? "OK" : "NOT OK")
				<< endl;
		if (response > 0)
			continue;

		//send image

		unsigned int dataSent = 0;
		while (dataSent < imagesize) {
			//std::cout << "Byte " << dataSent << endl;
			send(sockfd, &(contents[dataSent]), sizeof(contents[0]), 0);
			dataSent++;
			if (dataSent >= imagesize) break;
			//std::cout << "Sent " << bytesIn << " bytes." << endl;
		}


		cout << "sent image data: " << contents << endl;

		//free dat RAM
		delete[] contents;

		//wait for server response code
		recv(sockfd, &response, 4, 0);
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
		if (response > 0)
			continue;

		//
		recv(sockfd, &datasize, 4, 0);
		cout << "expecting data of length " << datasize << endl;

		data = "";
		readSize = 0;
		do {
			readSize += recv(sockfd, &tempdata, datasize, 0);
			data += tempdata;
			tempdata = "";
		} while (readSize < datasize);

		//account for the possibility of data getting smeared across packets

		//}

//		std::cout << "Sending... ";
//		send(sockfd, &kbinput, sizeof(kbinput), 0);
//		std::cout << "Sent." << endl;
//
//		std::cout << "Receiving... ";
//		readSize = recv(sockfd, &echoinput, sizeof(echoinput), 0);
//		std::cout << "Received." << endl;

		if (readSize == 0) {
			cout << "connection closed by server." << endl;
			break;
		} else {
			//cout << "Bytes Received: ";
			//cout << readSize;
			//cout << " Data: "<<endl;
			//cout << echoinput << endl;
			//sprintf(kbinput,"%l", random());
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
/*
 void receive(int __fd, void *__buf, size_t __n){
 string tempdata="";
 int readSize = 0;

 do{
 readSize+=recv(__fd, &tempdata, __n, 0);
 __buf += tempdata;
 tempdata = "";
 }while(readSize<__n);
 }

 void sanitize_path(string *path){
 string cwd;
 getcwd(&cwd, 200);
 path->=='.'
 }

 */
