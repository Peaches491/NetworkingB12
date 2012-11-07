/*
 * QTServer.h
 *
 *  Created on: Nov 5, 2012
 *      Author: cs3516
 */

#ifndef QTSERVER_H_
#define QTSERVER_H_


enum messageCode {TEXT, IMAGE, STATUS};
enum statusCode {TIME_OUT, MAX_USERS, TOO_BIG};

#define PARSED_DATA_MARKER string("Parsed result:\n")
#define ZXING_PATH         "java -cp javase.jar:core.jar com.google.zxing.client.j2se.CommandLineRunner "


#endif /* QTSERVER_H_ */
