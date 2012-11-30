#ifndef CS3516SOCK_H_
#define CS3516SOCK_H_

// Set the following port to a unique number:
#define MYPORT 58008

int create_cs3516_socket();
int cs3516_recv(int sock, char *buffer, int buff_size);
int cs3516_send(int sock, char *buffer, int buff_size, unsigned long nextIP);


#endif
