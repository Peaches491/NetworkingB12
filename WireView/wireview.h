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

int packetCount = 0; 					// Total number of packets.
timeval* startTime = new timeval;						// Start date and time of the packet capture.
timeval* durationTime = new timeval;					// Duration of the packet capture in seconds with microsecond resolution.
std::map<std::string, int> ipSrcMap;	// Create two lists, one for unique senders and one for unique recipients, along with the total number of packets associated with each.
std::map<std::string, int> ipDstMap;	//    This should be done at two layers: Ethernet and IP. For Ethernet, represent the addresses in hex-colon notation. For IP addresses, use the standard dotted decimal notation.
std::map<std::string, int> ethSrcMap;
std::map<std::string, int> ethDstMap;

std::map<std::string, int> UDPsrcPorts;
std::map<std::string, int> UDPdstPorts;
std::map<std::string, int> ARPlist;

// list of machines participating in ARP,
// 		including their associated MAC addresses and,
// 		where possible, the associated IP addresses.
// For UDP, create two lists for the unique ports seen: one for the source ports and one for the destination ports.
// Report the average, minimum, and maximum packet sizes. The packet size refers to everything beyond the tcpdump header.

void printStats();
int printMap(std::map<std::string, int> map, std::string format, int bufSize);
int processPCAP(pcap_t* cap);
char* niceIPaddr(in_addr* addr);
char* niceIPaddr(in_addr* addr, bool print);
char* niceMACaddr(u_int8_t addr[6]);
char* niceMACaddr(u_int8_t addr[6], bool print);
void handler(u_char *user, struct pcap_pkthdr *phrd, u_char *pdata);
int timeCompare(timeval* x, timeval* y);

char* TCPpayload(const u_char* pdata, u_int size_ip);


#endif
