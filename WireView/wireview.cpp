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

int processPCAP(pcap_t* cap);

int main(int argc, char *argv[]) {

	if(argc != 2){
		cout << "Usage: wireview <path to pcap file>" << endl;
		exit(1);
	}

	char errbuf[128];
	memset(errbuf, 0, sizeof(errbuf));
	pcap_t* result = pcap_open_offline(argv[1], errbuf);

	if(errbuf[0] != 0){
		cout << errbuf << endl;
		exit(1);
	}

	processPCAP(result);

	return 0;
}

int processPCAP(pcap_t* cap){
	cout << "pcap_datalink: ";
	if(pcap_datalink(cap) != DLT_EN10MB) {
		cout << "File not captured over ethernet. Exiting." << endl;
		exit(1);
	} else {
		cout << "File captured over Ethernet." << endl;
	}

	// this is more or less how you get a packet via libpcap
	pcap_pkthdr *header = new pcap_pkthdr;
	const u_char *packet = pcap_next(cap, header);

	return 0;
}

//create a traditional human-readable IPv4 address
char* niceIPaddr(u_int32_t addr){
	char* ip = new char[15];

	sprintf(ip, "%d.%d.%d.%d", (addr&0xFF000000)>>24, (addr&0x00FF0000)>>16, (addr&0x0000FF00)>>8, (addr&0x000000FF));

	return ip;
}

//create a traditional human-readable MAC address
char* niceMACaddr(u_int8_t addr[6]){
	char* mac = new char[17];

	sprintf(mac, "%x:%x:%x:%x:%x:%x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	return mac;
}
