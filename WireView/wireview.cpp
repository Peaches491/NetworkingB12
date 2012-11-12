#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <pcap/pcap.h>
#include "wireview.h"

#include <net/ethernet.h>
#include <net/if_arp.h>

//#include <netinet/if_ether.h>	//ethernet ARP
#include <netinet/ether.h>
#include <netinet/in.h>		//port definitions, may well not need these
#include <netinet/ip.h>		//IP
#include <netinet/tcp.h>	//
/*
 useful stuff:
 net/ethernet.h:
 struct ether_header

 net/if_arp.h:
 struct arphdr

 netinet/ether.h:
 char *ether_ntoa (__const struct ether_addr *__addr)

 netinet/in.h:
 struct in_addr

 netinet/ip.h:
 struct ip

 netinet/tcp.h:
 struct tcphdr
 */

using namespace std;

int main(int argc, char *argv[]) {

	if (argc != 2) {
		cout << "Usage: wireview <path to pcap file>" << endl;
		exit(1);
	}

	char errbuf[128];
	memset(errbuf, 0, sizeof(errbuf));
	pcap_t* result = pcap_open_offline(argv[1], errbuf);

	if (errbuf[0] != 0) {
		cout << errbuf << endl;
		exit(1);
	}

	processPCAP(result);
	printStats();

	return 0;
}

void printStats() {
	string prefix("    ");

	cout << endl;
	printf("Total number of Packets: %i\n", packetCount);
	printf("Start Time: %li\n", (long int) startTime.tv_sec);

	cout << "Ethernet Source Statistics" << endl;
	map<string, int>::const_iterator iter;
	for (iter = ethSrcMap.begin(); iter != ethSrcMap.end(); ++iter) {
		cout << prefix << iter->first << "\t" << iter->second << endl;
	}

	cout << "Ethernet Destination Statistics" << endl;
	for (iter = ethDstMap.begin(); iter != ethDstMap.end(); ++iter) {
		cout << prefix << iter->first << "\t" << iter->second << endl;
	}

	cout << "IP Source Statistics" << endl;
	for (iter = ipSrcMap.begin(); iter != ipSrcMap.end(); ++iter) {
		cout << prefix << iter->first << "\t" << iter->second << endl;
	}

	cout << "IP Destination Statistics" << endl;
	for (iter = ipDstMap.begin(); iter != ipDstMap.end(); ++iter) {
		cout << prefix << iter->first << "\t" << iter->second << endl;
	}
}

int processPCAP(pcap_t* cap) {
	cout << "pcap_datalink: ";
	if (pcap_datalink(cap) != DLT_EN10MB) {
		cout << "File not captured over ethernet. Exiting." << endl;
		exit(1);
	} else {
		cout << "File captured over Ethernet." << endl;
	}

	// this is more or less how you get a packet via libpcap
	//pcap_pkthdr *header = new pcap_pkthdr;
	//const u_char *packet = pcap_next(cap, header);
	int result = pcap_loop(cap, -1, (pcap_handler) &handler, (u_char*) "");
	return result;
}

void handler(u_char* user, struct pcap_pkthdr* phrd, u_char* pdata) {
	packetCount++;
	timeval* res = new timeval;

	// If this packet capture time is older than the start time, update the start time
	//if (timeCompare(&(phrd->ts), &(), res)) {
	//	startTime = phrd->ts;
	//}

	const ether_header* ethernet = (ether_header*) (pdata);
	cout << niceMACaddr((uint8_t*) (ethernet->ether_dhost)) << endl;
	ethDstMap[niceMACaddr((uint8_t*) (ethernet->ether_dhost))]++;

	cout << niceMACaddr((uint8_t*) (ethernet->ether_shost)) << endl;
	ethSrcMap[niceMACaddr((uint8_t*) (ethernet->ether_shost))]++;

	const ip* ip_header = (ip*) (pdata + sizeof(ether_header));
	cout << niceIPaddr((in_addr*)&(ip_header->ip_dst)) << endl;
	ipDstMap[niceIPaddr((in_addr*)&(ip_header->ip_dst))]++;

	cout << niceIPaddr((in_addr*)&(ip_header->ip_src)) << endl;
	ipSrcMap[niceIPaddr((in_addr*)&(ip_header->ip_src))]++;

	unsigned int size_ip = (ip_header->ip_hl) * 4; //header length //size_ip = IP_HL(ip)*4;

	if (size_ip < 20) {
		printf("\t* Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	//////////////////protocol checking
	if (ip_header->ip_p == IPPROTO_TCP) {
		//printf("\t* Found TCP packet\n");
		//cout << TCPpayload(pdata, size_ip) << endl;
	}

	//if(ip->protocol == IPPro)
}

char* TCPpayload(const u_char* pdata, u_int size_ip) {
	tcphdr * tcp_header = (tcphdr*) (pdata + sizeof(ether_header) + size_ip);

	u_int size_tcp = (tcp_header->doff) * 4;

	char* payload;

	if (size_tcp < 20) {
		printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
	} else {
		payload = (char *) (pdata + sizeof(ether_header) + size_ip + size_tcp);
	}

	return payload;
}

int timeCompare(timeval* x, timeval* y, timeval* result) {
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

//create a traditional human-readable IPv4 address
char* niceIPaddr(in_addr* addr) {
	char* ip = new char[15];
	memset(ip, 0, sizeof(char) * 15);

	inet_ntop(AF_INET, addr, ip, 15);

	return ip;
}

//create a traditional human-readable MAC address
char* niceMACaddr(u_int8_t addr[6]) {
	char* mac = new char[17];
	memset(mac, 0, sizeof(char) * 17);

	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2],
			addr[3], addr[4], addr[5]);

	return mac;
}
