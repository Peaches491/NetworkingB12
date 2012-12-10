# makefile
# 
# Daniel Miller- MillerD
# Kevin Janesch
# Project 3



CFLAGS = -g -pthread -std=c++0x -Wall
#CFLAGS = -g -Wall  



all: node
	
		
node: node.o host.o router.o packet.o cs.o tree.o pq.o logger.o
	g++ node.o router.o host.o packet.o cs.o tree.o pq.o logger.o $(CFLAGS) -o node

	
	
node.o: Node.cpp 
	g++ -c $(CFLAGS) Node.cpp -o node.o

router.o: Router.cpp 
	g++ -c $(CFLAGS) Router.cpp -o router.o

logger.o: PacketLogger.cpp 
	g++ -c $(CFLAGS) PacketLogger.cpp -o logger.o
			
host.o: Host.cpp 
	g++ -c $(CFLAGS) Host.cpp -o host.o
	
packet.o: Packet.cpp 
	g++ -c $(CFLAGS) Packet.cpp -o packet.o
	
cs.o: cs3516sock.cpp 
	g++ -c $(CFLAGS) cs3516sock.cpp -o cs.o

tree.o: LPMTree.cpp 
	g++ -c $(CFLAGS) LPMTree.cpp -o tree.o

pq.o:  
	g++ -c $(CFLAGS) PacketQueue.cpp -o pq.o	
	


clean:
	rm -f *.o node


