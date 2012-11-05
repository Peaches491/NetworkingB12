/*
 * QTSession.h
 *
 *  Created on: Oct 31, 2012
 *      Author: cs3516
 */

#ifndef QTSESSION_H_
#define QTSESSION_H_

#include <string>

#define PARSED_DATA_MARKER "Parsed result:\n"
#define ZXING_PATH         "java -cp javase.jar:core.jar com.google.zxing.client.j2se.CommandLineRunner "

class QTSession {
public:
	QTSession(int sockfd);
	std::string getCode(); //Send confirmation. Recieve QR Code data. return path to code
	std::string interpretCode();	// exec java with path to image file.
	virtual ~QTSession();
private:
	int dataSocket;
	int receiveState;

	static const int STATE_WAIT_FOR_SIZE  = 1;
	static const int STATE_WAIT_FOR_IMAGE = 2;
	//static std::string ZXING_PATH;
};

#endif /* QTSESSION_H_ */
