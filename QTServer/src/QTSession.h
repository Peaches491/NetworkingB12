/*
 * QTSession.h
 *
 *  Created on: Oct 31, 2012
 *      Author: cs3516
 */

#ifndef QTSESSION_H_
#define QTSESSION_H_

#include <string>

class QTSession {
public:
	QTSession(int sockfd);
	std::string getCode(); //Send confirmation. Recieve QR Code data. return path to code
	std::string interpretCode();	// exec java with path to image file.
	virtual ~QTSession();
private:
	int dataSocket;
};

#endif /* QTSESSION_H_ */
