// references:
// 		wikipedia pages for ethernet frame, ARP frame, TCP frame
// 		sniffex.c from http://www.tcpdump.org/pcap.htm

#include <pcap.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <map>

#ifndef _WIREVIEW_H
#define _WIREVIEW_H

#define MAC_LENGTH 6

#define ETHERNET_LENGTH 14

typedef std::map<int, int> int_int_map;
typedef std::map<std::string, int> string_int_map;
typedef std::map<std::string, std::map<std::string, int> > string_string_int_map;

int packetCount = 0; 					// Total number of packets.
int minPacketSize = 0x7fffffff;
int maxPacketSize = 0;
long int packetSizeSum = 0;
timeval* startTime = new timeval;		// Start date and time of the packet capture.
timeval* endTime = new timeval;
timeval* durationTime = new timeval;	// Duration of the packet capture in seconds with microsecond resolution.
string_int_map ipSrcMap;	// Create two lists, one for unique senders and one for unique recipients, along with the total number of packets associated with each.
string_int_map ipDstMap;	// This should be done at two layers: Ethernet and IP. For Ethernet, represent the addresses in hex-colon notation. For IP addresses, use the standard dotted decimal notation.
string_int_map ethSrcMap;
string_int_map ethDstMap;


int_int_map udpSrcPortsMap;
int_int_map udpDstPortsMap;
string_string_int_map arpReqMap;
string_string_int_map arpRcvMap;

// list of machines participating in ARP,
// 		including their associated MAC addresses and,
// 		where possible, the associated IP addresses.
// For UDP, create two lists for the unique ports seen: one for the source ports and one for the destination ports.
// Report the average, minimum, and maximum packet sizes. The packet size refers to everything beyond the tcpdump header.

void printTime(timeval* time);
void printTimeSeconds(timeval* time);
void printStats();
int printMap(std::map<std::string, int> map, std::string format, int bufSize);
int printMapInt(std::map<int, int> map, std::string format, int bufSize);
int printMapMap(std::map<std::string, std::map<std::string, int> > map, std::string format, int bufSize);
int processPCAP(pcap_t* cap);
char* niceIPaddr(in_addr* addr);
char* niceIPaddr(in_addr* addr, bool print);
char* niceIPaddrA(u_int8_t* nums);
char* niceMACaddr(u_int8_t addr[6]);
char* niceMACaddr(u_int8_t addr[6], bool print);
void handler(u_char *user, struct pcap_pkthdr *phrd, u_char *pdata);
int timeCompare(timeval* x, timeval* y);

char* TCPpayload(const u_char* pdata, u_int size_ip);


#endif
