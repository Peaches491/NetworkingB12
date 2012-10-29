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

	//... some more code
	std::cin.get();
}
//}

