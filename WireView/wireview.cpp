#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <pcap/pcap.h>

using namespace std;

int processPCAP(pcap_t* cap);

int main(int argc, char *argv[]) {

	if(argc != 2){
		cout << "Usage: wireshark <path to pcap file>" << endl;
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

	return 0;
}
