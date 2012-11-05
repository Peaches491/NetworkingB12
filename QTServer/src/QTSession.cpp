#include "QTSession.h"
#include <string>
#include <iostream>

using std::string;

QTSession::QTSession(int sockfd) {
	this->dataSocket = sockfd;

	std::cout << QTSession::getCode() << "\n";
}

QTSession::~QTSession() {
	// delete image file
}

//Send confirmation. Recieve QR Code data. return path to code
string QTSession::getCode(){
	return string("Your Mom!");
}

// exec java with path to image file.
string QTSession::interpretCode(){
	return string("Your Dad!");
}
