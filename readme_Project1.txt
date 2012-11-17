Authors:                        Daniel Miller and Kevin Janesch
Date:                           11/6/2012
Project ID:                     Project 1 - QR Code Server
CS Class:                       CS 3516
Programming Language:           C++
OS/Hardware Dependencies:       Linux (tested on Ubuntu 12.04, kernel 3.2.0-32 i686)

Problem Description:            Create a QR code server that can support multiple 
                                  connections, and a client to send QR code images
                                  to the server and fetch their decoded contents.

Overall Design:                 
        System structure        The system consists of a client and server program.
                                  Once the server is started, clients can connect
                                  via TCP sockets and send images representing QR
                                  codes. 
                                Each connection on the server side creates a new 
                                  process tied to that client
                                After a given amount of inactivity on a given 
                                  connection, the server terminates the session.
                                  
Program Assumptions 
      and Restrictions:         
                                Restrictions are in place on the maximum number of
                                  connected clients, the maximum rate at which 
                                  QR codes can be interpreted by the server, and 
                                  the size of the image file that can be submitted. 
                                Client and server are not invulnerable to other 
                                  attacks, though. Whatever they may be.

Program usage:  
                                ./QRServer -p [PORT] -r [MAX_REQUEST_RATE] 
                                        -u [MAX_USERS] -t [CONNECTION_TIMEOUT]
                                
                                ./QRClient -p [PORT] -a [SERVER_ADDRESS]
                                
Additional files:               Uses javase.jar and core.jar 
                                This program uses the Java-based ZXing QR code 
                                  interpreter (http://code.google.com/p/zxing/) as
                                  compiled into two 
