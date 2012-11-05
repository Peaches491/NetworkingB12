#include "QTSession.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>

using std::string;

QTSession::QTSession(int sockfd) {
	this->dataSocket = sockfd;
	receiveState = STATE_WAIT_FOR_SIZE;

	std::cout << QTSession::getCode() << "\n";
}

QTSession::~QTSession() {
	// delete image file
}

//should something go in here to sit on recv() and wait for data?

//Send confirmation. Recieve QR Code data. return path to code
string QTSession::getCode(){
	return string("Your Mom!");
}

// exec java with path to image file.
string QTSession::interpretCode(){

	FILE *fpipe;
	char line[256];

	//TODO: concatenate the rest of the path on to here
	string command = string(ZXING_PATH)+"";
	string output;
	string interpretedCode;
	bool storeNextLine = 0;

	//this'll call the ZXing command
	// but it'll also allow us to crawl the output
	// for the delicious interpreted QR code
	if(!(fpipe=(FILE*)popen(command.c_str(), "r"))){
		perror("Problem executing QR code interpreter");
		//exit(1);
	}

	//read in ALL the lines!
	while(fgets(line, sizeof line, fpipe)){
		if(storeNextLine==1){
			interpretedCode = string(line);
			storeNextLine = -1;// disable any further writes to the string
		}
		//store all the output for logging.
		//TODO: Probably wanna add this to the log. Maybe just swap this line for that?
		output += string(line)+"\n";

		//will flip to one when not storing a line and the current line marks the data
		storeNextLine = storeNextLine==0 && strcmp(line, PARSED_DATA_MARKER)==0;
	}

	//close the stream/"file"/command
	pclose(fpipe);

	//no interpreted code was found.
	if(interpretedCode=="")
		std::cerr<<"An error occurred when trying to interpret the QR code.";

	//return string("Your Dad!");
	return interpretedCode;
}
