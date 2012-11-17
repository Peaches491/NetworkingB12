#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <pcap/pcap.h>
#include <map>
#include <sstream>
#include <cmath>
#include "wireview.h"

#include <net/ethernet.h>
#include <net/if_arp.h>

#include <netinet/if_ether.h>	//ethernet ARP
#include <netinet/ether.h>
#include <netinet/in.h>		//port definitions, may well not need these
#include <netinet/ip.h>		//IP
#include <netinet/tcp.h>	//
#include <netinet/udp.h>
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

void printStats() {
	string prefix("    ");

	cout << endl;
	printf("Total number of Packets: %i\n", packetCount);
	printf("Maximum Packet Size: %i bytes\n", maxPacketSize);
	printf("Minimum Packet Size: %i bytes\n", minPacketSize);
	printf("Average Packet Size: %i\n\n", (int)round(((float)(packetSizeSum)) / ((float)(packetCount))));

	printf("Start Time: ");
	printTime(startTime);
	printf("End Time: ");
	printTime(endTime);

	if ((endTime->tv_usec - startTime->tv_usec) > 0) {
		durationTime->tv_sec = endTime->tv_sec - startTime->tv_sec;
		durationTime->tv_usec = endTime->tv_usec - startTime->tv_usec;
	} else {
		durationTime->tv_sec = (endTime->tv_sec - startTime->tv_sec) - 1;
		durationTime->tv_usec = (endTime->tv_usec + 1000000)
				- startTime->tv_usec;
	}

	printf("Capture Duration: ");
	printTimeSeconds(durationTime);
	printf("\n");

	cout << "\nEthernet Source Statistics" << endl;
	printMap(ethSrcMap, "    %-17s \t\t%i", 50);

	cout << "\nEthernet Destination Statistics" << endl;
	printMap(ethDstMap, "    %-17s \t\t%i", 50);

	cout << "\nIP Source Statistics" << endl;
	printMap(ipSrcMap, "    %-15s \t\t%i", 50);

	cout << "\nIP Destination Statistics" << endl;
	printMap(ipDstMap, "    %-15s \t\t%i", 50);

	cout << "\nUDP Source Ports" << endl;
	printMapInt(udpSrcPortsMap, "    %-5s \t\t\t%i", 50);

	cout << "\nUDP Destination Statistics" << endl;
	printMapInt(udpDstPortsMap, "    %-5s \t\t\t%i", 50);

	cout << "\nARP Request Statistics" << endl;
	printMapMap(arpReqMap, "    %-17s \t%i", 50);

	cout << "\nARP Receive Statistics" << endl;
	printMapMap(arpRcvMap, "    %-17s \t%i", 50);
}

void handler(u_char* user, struct pcap_pkthdr* pkthdr, u_char* pdata) {
	packetCount++;
	int packetSize = pkthdr->len;

	if(packetSize > maxPacketSize){
		maxPacketSize = packetSize;
	}
	if(packetSize < minPacketSize){
		minPacketSize = packetSize;
	}
	packetSizeSum += packetSize;

	if (startTime->tv_sec == 0) {
		memcpy(startTime, &(pkthdr->ts), sizeof(timeval));
	} else if (timeCompare(&(pkthdr->ts), (timeval*) startTime) == 1) {
		memcpy(startTime, &(pkthdr->ts), sizeof(timeval));
	}

	if ((endTime->tv_sec == 0) && (endTime->tv_usec == 0)) {
		memcpy(endTime, &(pkthdr->ts), sizeof(timeval));
	} else if (timeCompare((timeval*) endTime, &(pkthdr->ts))) {
		memcpy(endTime, &(pkthdr->ts), sizeof(timeval));
	}

	const ether_header* ethernet = (ether_header*) (pdata);
	ethDstMap[niceMACaddr((uint8_t*) (ethernet->ether_dhost), false)]++;
	ethSrcMap[niceMACaddr((uint8_t*) (ethernet->ether_shost), false)]++;

	int type = ntohs(ethernet->ether_type);

	const ip* ip_header = (ip*) (pdata + sizeof(ether_header));
	const ether_arp* arp_header = (ether_arp*) (pdata + sizeof(ether_header));
	const udphdr* udp_header = (udphdr*) (pdata + sizeof(ether_header) + sizeof(ip));

	char* addr = niceIPaddr((in_addr*) &(ip_header->ip_dst), false);

	switch (type) {
	case ETHERTYPE_ARP:
		//printf("ARP\n");

		arpReqMap[niceMACaddr((u_int8_t*) arp_header->arp_sha)][niceIPaddrA(
				(u_int8_t*) (arp_header->arp_spa))]++;
		arpRcvMap[niceMACaddr((u_int8_t*) arp_header->arp_tha)][niceIPaddrA(
				(u_int8_t*) (arp_header->arp_tpa))]++;

		break;
	case ETHERTYPE_IP:
		addr = niceIPaddr((in_addr*) &(ip_header->ip_dst), false);
		ipDstMap[addr]++;
		addr = niceIPaddr((in_addr*) &(ip_header->ip_src), false);
		ipSrcMap[addr]++;

		switch (ip_header->ip_p) {
		case IPPROTO_UDP:
			udpSrcPortsMap[ntohs(udp_header->source)]++;
			udpDstPortsMap[ntohs(udp_header->dest)]++;
			break;
		case IPPROTO_TCP:
		default:
			break;
		}
		break;
	default:
		printf("~else~\n");
		break;
	}

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

int timeCompare(timeval* x, timeval* y) {
	if ((x->tv_sec == y->tv_sec) && (x->tv_usec == y->tv_usec)) {
		return 0;
	} else if ((x->tv_sec == y->tv_sec)) {
		if ((x->tv_usec < y->tv_usec)) {
			return 1;
		} else {
			return -1;
		}
	} else if (x->tv_sec < y->tv_sec) {
		return 1;
	} else {
		return -1;
	}

}

//create a traditional human-readable IPv4 address
char* niceIPaddr(in_addr* addr) {
	return niceIPaddr(addr, false);
}
char* niceIPaddr(in_addr* addr, bool print) {
	char* ip = new char[16];
	memset(ip, 0, sizeof(ip));

	inet_ntop(AF_INET, addr, ip, 16);

	if (print) {
		cout << ip << endl;
	}

	return ip;
}
char* niceIPaddrA(u_int8_t* nums) {
	char* ip = new char[16];
	memset(ip, 0, sizeof(ip));
	sprintf(ip, "%i.%i.%i.%i", nums[0], nums[1], nums[2], nums[3]);
	//printf("%s", ip);
	return ip;
}

//create a traditional human-readable MAC address
char* niceMACaddr(u_int8_t addr[6]) {
	return niceMACaddr(addr, false);
}
char* niceMACaddr(u_int8_t addr[6], bool print) {
	char* mac = new char[17];
	memset(mac, 0, sizeof(char) * 17);

	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2],
			addr[3], addr[4], addr[5]);

	if (print) {
		cout << mac << endl;
	}

	return mac;
}

int printMap(map<string, int> map, string format, int bufSize) {
	int i = 0;
	char* buffer = new char[bufSize];

	string_int_map::const_iterator iter;
	for (iter = map.begin(); iter != map.end(); ++iter) {
		snprintf(buffer, bufSize, format.c_str(), iter->first.c_str(),
				iter->second);
		cout << buffer << endl;
		i += iter->second;
	}
	snprintf(buffer, bufSize, format.c_str(), "  Total", i);
	cout << buffer << endl;
	return i;
}

int printMapInt(map<int, int> map, string format, int bufSize) {
	int i = 0;
	char* buffer = new char[bufSize];

	int_int_map::const_iterator iter;
	for (iter = map.begin(); iter != map.end(); ++iter) {
		stringstream ss;
		ss << iter->first;
		snprintf(buffer, bufSize, format.c_str(), (char*)ss.str().c_str(),
				iter->second);
		cout << buffer << endl;
		i += iter->second;
	}
	snprintf(buffer, bufSize, format.c_str(), "  Total", i);
	cout << buffer << endl;
	return i;
}


int printMapMap(std::map<std::string, std::map<std::string, int> > map,
		std::string format, int bufSize) {
	int macs = 0;
	int addresses = 0;
	char* buffer = new char[bufSize];

	string_string_int_map::const_iterator iter_out;
	for (iter_out = map.begin(); iter_out != map.end(); ++iter_out) {

		addresses = 0;

		snprintf(buffer, bufSize, "    %s", iter_out->first.c_str());
		cout << buffer << endl;

		string_int_map::const_iterator iter_in;
		string_int_map inside_map = iter_out->second;
		for (iter_in = inside_map.begin(); iter_in != inside_map.end();
				++iter_in) {
			snprintf(buffer, bufSize, ("  " + format).c_str(),
					iter_in->first.c_str(), iter_in->second);
			cout << buffer << endl;
			addresses += iter_in->second;
		}
		snprintf(buffer, bufSize, ("  " + format).c_str(), "  IPs ", addresses);
		cout << buffer << endl << endl;
		macs++;
	}
	snprintf(buffer, bufSize, ("  " + format).c_str(), "  MACs ", macs);
	cout << buffer << endl;
	return macs;
}

void printTime(timeval* time) {
	char timeBuf[56];
	memset(timeBuf, 0, sizeof(timeBuf));
	strftime(timeBuf, sizeof(timeBuf), "%a %B %d, %Y %X",
			localtime((time_t*) &(time->tv_sec)));
	printf("%s.%li\n", timeBuf, time->tv_usec);
}

void printTimeSeconds(timeval* time) {
	printf("%li.%li sec", time->tv_sec, time->tv_usec);
}
