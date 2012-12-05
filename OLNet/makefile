# makefile
# 
# Daniel Miller- MillerD
# Kevin Janesch
# Project 3



CFLAGS = -lpcap -g -Wall 



all: node
	
		
node: node.o host.o router.o packet.o cs.o
	g++ node.o router.o host.o packet.o cs.o $(CFLAGS) -o node
	
	
	
node.o: Node.cpp 
	g++ -c $(CFLAGS) Node.cpp -o node.o

router.o: Router.cpp 
	g++ -c $(CFLAGS) Router.cpp -o router.o
	
host.o: Host.cpp 
	g++ -c $(CFLAGS) Host.cpp -o host.o
	
packet.o: Packet.cpp 
	g++ -c $(CFLAGS) Packet.cpp -o packet.o
	
cs.o: cs3516sock.cpp 
	g++ -c $(CFLAGS) cs3516sock.cpp -o cs.o



clean:
	rm -f *.o node


