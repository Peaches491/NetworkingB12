# makefile
# 
# Daniel Miller- MillerD
# Kevin Janesch
# Project 2
# 11/10/2012
# 
# This makefile enables the compilation of the 
#    program 'Wireview' which is used to 
#    analyze the contents of the PCAP file
#    specified as the first argument to the 
#    program


CFLAGS = -lpcap -g -Wall 



all: wireview
		
wireview: wireview.o
	g++ wireview.o $(CFLAGS) -o wireview
	
wireview.o: wireview.cpp wireview.h
	g++ -c $(CFLAGS) wireview.cpp


clean:
	rm -f *.o wireview

