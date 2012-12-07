/*
 * Host.h
 *
 *  Created on: Nov 30, 2012
 *      Author: cs3516
 */

#ifndef HOST_H_
#define HOST_H_

int runHost(in_addr* ip, char* file, char* destIPl, char* routerIP, int ttl);
int getFileSize(FILE* fd);

#endif /* HOST_H_ */
