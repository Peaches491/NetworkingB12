# makefile
# 
# Daniel Miller- MillerD
# Kevin Janesch
# Project 3



CFLAGS = -lpcap -g -Wall 



all: router host
	
		
router: router.o
	g++ router.o $(CFLAGS) -o router
	
router.o: OLRouter.cpp 
	g++ -c $(CFLAGS) OLRouter.cpp -o router.o


host: host.o
	g++ host.o $(CFLAGS) -o host
	
host.o: OLHost.cpp 
	g++ -c $(CFLAGS) OLHost.cpp -o host.o

clean:
	rm -f *.o router host


