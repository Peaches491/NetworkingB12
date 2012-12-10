Authors:                        Daniel "Token Lord" Miller and Kevin "Is There Actually Anyone Here Named 'Kevin'?" Janesch
Date:                           12/09/2012
Project ID:                     Project 3
CS Class:                       CS 3516
Programming Language:           C++
OS/Hardware Dependencies:       Linux (tested on Ubuntu 12.04, kernel 3.2.0-32 i686)

Problem Description:            Create a UDP-over-IP network on top of existing 
                                  UDP packets.  Includes routing packets between
                                  hosts and other routers, complete with TTL, queue
                                  lengths, and transmission delays. 

Overall Design:                 An initial setup routine reads the network map
                                  from config.txt and starts the process for 
                                  either the router or host based on the command
                                  line arguments. 
                                For hosts, the file send_config.txt is read in,
                                  indicating where the data must be sent. The host
                                  also opens the file send_body and transmits its
                                  contents to the destination specified in 
                                  send_config.txt.
                                
Program Assumptions 
      and Restrictions:         All communications take place on ports defined in
                                  send_config.txt 

Implementation details:         Each instance reads from the "config.txt" file in 
                                  the executable's directory for the network map
                                  information.
                                Each router has a queue length and transmission
                                  delay, and will begin to drop packets once the 
                                  queue is filled.
                                Each host determines its send destination via the
                                  send_config.txt file, and transmits the length, then
                                  contents of send_body to the destination. If 
                                  send_body is larger than 1000 bytes, the file
                                  is sent in 1000-byte fragments until the end of
                                  the file is reached. 
                                Data is encapsulated in a UDP packet and an IPv4
                                  packet in software, which is then encapsulated
                                  in UDP, IPv4, and ethernet packets at the kernel
                                  level. Packets are received at the kernel level,
                                  the software-level IP and UDP data is manipulated,
                                  and the packets are transmitted to the next node
                                  by the kernel-level code again.

Program usage:                  ./node [options]

                                    Available Options:
                                        -r                 Node behaves as a router
                                        -h                 Node behaves as a host

To build:                       make clean && make

Dependencies:                   None.

send_config.txt format:
        DESTINATION_OVERLAY_IP_ADDRESS SOURCE_OVERLAY_PORT DESTINATION_OVERLAY_PORT

config.txt format:
        0 QUEUE_LENGTH DEFAULT_TTL_VALUE                                                (Global configuration options)
        1 ROUTER_ID REAL_NETWORK_IP                                                     (Router configuration)
        2 END_HOST_ID REAL_NETWORK_IP HOST_OVERLAY_IP                                   (Host Configuration)
        3 ROUTER_1_ID ROUTER_1_SEND_DELAY ROUTER_2_ID ROUTER_2_SEND_DELAY               (Router-to-router link)
        4 ROUTER_ID ROUTER_SEND_DELAY OVERLAY_PREFIX END_HOST_ID HOST_SEND_DELAY        (Router-to-host link)

