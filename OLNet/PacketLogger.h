/*
 * Log.h
 *
 *  Created on: Dec 8, 2012
 *      Author: cs3516
 */

#ifndef LOG_H_
#define LOG_H_

#include <map>
#include <sstream>
#include <fstream>
#include <string.h>
#include <strings.h>

#include "Packet.h"

namespace std {

class PacketLogger {
public:
	enum LogType {
		TTL_EXPIRED, MAX_SENDQ_EXCEEDED, NO_ROUTE_TO_HOST, SENT_OKAY
	};

	PacketLogger() {
		typeString[TTL_EXPIRED] = string("TTL_EXPIRED");
		typeString[MAX_SENDQ_EXCEEDED] = string("MAX_SENDQ_EXCEEDED");
		typeString[NO_ROUTE_TO_HOST] = string("NO_ROUTE_TO_HOST");
		typeString[SENT_OKAY] = string("SENT_OKAY");

		fileLock = PTHREAD_MUTEX_INITIALIZER;
	}

	void initLogger();
	void closeLogger();
	void logPacket(packethdr* p, LogType type);
	void logPacket(packethdr* p, LogType type, in_addr nextHop);
	std::map<int, std::string> getTypes() {
		return typeString;
	};
	ofstream logF;
	pthread_mutex_t fileLock;
private:
	std::map<int, std::string> typeString;
};
}

extern std::PacketLogger globalLog;

#endif /* LOG_H_ */
