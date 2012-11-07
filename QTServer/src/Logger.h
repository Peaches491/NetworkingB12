/*
 * Logger.h
 *
 *  Created on: Nov 6, 2012
 *      Author: cs3516
 */

#ifndef LOGGER_H_
#define LOGGER_H_


void log(const char* mesg);
void log(struct sockaddr* sock, const char* mesg);


std::string getTimestamp();
std::string getAddr(struct sockaddr *sock);


#endif /* LOGGER_H_ */
