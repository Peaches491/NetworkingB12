# makefile
# 
# Daniel Miller- MillerD
# Kevin Janesch
# Project 3



CFLAGS = -lpcap -g -Wall 



all: router
	
		
router: router.o
	g++ router.o $(CFLAGS) -o router
	
router.o: OLRouter.cpp 
	g++ -c $(CFLAGS) OLRouter.cpp -o router.o


clean:
	rm -f *.o router host


