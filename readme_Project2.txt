Authors:                        Daniel Miller and Kevin Janesch
Date:                           11/16/2012
Project ID:                     Project 2
CS Class:                       CS 3516
Programming Language:           C++
OS/Hardware Dependencies:       Linux (tested on Ubuntu 12.04, kernel 3.2.0-32 i686)

Problem Description:            Create a tcpdump/wireshark-like program that can 
                                  analyze IPv4 UDP-over-IP packets and ARP requests 
                                  pre-recorded from an Ethernet interface.

Overall Design:                 This program opens a specified .pcap file, and
                                  parses out IP addresses for packets sent over
                                  Ethernet. From there, packet headers are parsed to
                                  find ARP and UDP packets. For ARP packets, a list 
                                  of source and destination IP and MAC addresses is 
                                  maintained. For UDP packets, the packet sizes are
                                  tabulated to kind the max/min/average packet size 
                                  statistics.

Program Assumptions 
      and Restrictions:         Program only supports ethernet-based IPv4 ARP packets 
                                  and UDP over IP. 
                                No other link or network layers are supported, and 
                                  there is no support for any application layer
                                  protocols.

Implementation details:         To parse packets, a packet is stored at a given point
                                  in memory, and the pointer is advanced through the 
                                  packet to the expected location of the next layer's
                                  header.
                                MAC and IP addresses are tabulated in C++ maps to 
                                  allow for easier checking of existing of an address. 

Program usage:                  ./wireview [path to pcap file]

To build:                       make clean && make

Dependencies:                   Requires libpcap-dev package and all dependencies
                                  therein.

