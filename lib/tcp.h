#ifndef TCP_H
#define TCP_H

#include <stdio.h>

void modem_configure(void);

int cred_delete(int slot, int type);

int cred_exists(int slot, int type);

int recv_tcp(int fd, char* buf, int bufsize);

int send_tcp(int fd, char* buf, ssize_t len);

/* Setup TLS options on a given socket */
int tls_setup(int fd, int slot);
#endif