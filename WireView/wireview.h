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

/*
typedef struct {

} ip_addr;

//  Ethernet header
typedef struct {
	u_int8_t  dest[MAC_LENGTH];		//destination MAC
	u_int8_t  src[MAC_LENGTH];		//source MAC
	u_int16_t type;					//
} eth_header;

//  IP header
typedef struct {
	u_int8_t  vhl;					// version (high 4), header length (low 4)
	u_int8_t  type;					// type of service
	u_int16_t len;					// length
	u_int16_t id;					// ID
	u_int16_t offs;					// offset
	u_int8_t  ttl;					// time to live
	u_int8_t  prt;					// protocol
	u_int16_t sum;					// checksum
	u_int32_t src, dest;			// source and destination addresses

	//defines for fragment field
#define IP_RF		0x8000			// reserved fragment flag
#define IP_DF		0x4000			// don't fragment flag
#define IP_MF		0x2000			// more fragments
#define IP_OFFMASK	0x1FFF			// mask for fragmenting bits

#define IP_HEAD_LEN(h)	(((h)->vhl) & 0x0f)
#define IP_VERSION(h)	(((h)->vhl) >> 4)
} ip_header;

//	TCP header
typedef struct {
	u_int16_t src;					// source port
	u_int16_t dest;					// destination port
	u_int32_t seqn;					// sequence number
	u_int32_t ackn;					// acknowledgement number
	u_int8_t  offs;					// data offset (4 bits)
	u_int8_t  flags;				// TCP flags
	u_int16_t win;					// window length
	u_int16_t sum;					// checksum
	u_int16_t urgp;					// urgent pointer

#define TCP_OFFSET(h)	(((h)->offs  & 0xF0) >> 4)
#define TCP_NS(h)		(((h)->offs) & 0x01)
#define TCP_FIN 	0x01
#define TCP_SYN 	0x02
#define TCP_RST 	0x04
#define TCP_PUSH	0x08
#define TCP_ACK		0x10
#define TCP_URG 	0x20
#define TCP_ECE 	0x40
#define TCP_CWR 	0x80
#define TCP_FLAGS		(TCP_FIN|TCP_SYN|TCP_RST|TCP_PUSH|TCP_ACK|TCP_ACK|TCP_URG|TCP_ECE|TCP_CWR)

} tcp_header;
*/

int packetCount = 0; 	// Total number of packets.
timeval startTime;		// Start date and time of the packet capture.
timeval	durationTime;	// Duration of the packet capture in seconds with microsecond resolution.
std::map<std::string, int> ipSrcMap;
std::map<std::string, int> ipDstMap;
std::map<std::string, int> ethSrcMap;
std::map<std::string, int> ethDstMap;

// Create two lists, one for unique senders and one for unique recipients, along with the total number of packets associated with each.
//    This should be done at two layers: Ethernet and IP. For Ethernet, represent the addresses in hex-colon notation. For IP addresses, use the standard dotted decimal notation.

// Create a list of machines participating in ARP, including their associated MAC addresses and, where possible, the associated IP addresses.
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
