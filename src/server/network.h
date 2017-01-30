#ifndef _NETWORK_H_
#define _NETWORK_H_

/* includes */

#include <arpa/inet.h>

#include "server.h"

/* constants */

#define MS 1000

/* own types */

enum flags
{
	NOFLAGS = 0,
	BIND = 1,

};

typedef unsigned short port_t;
typedef struct message_t msg_t;
typedef struct conn_t conn_t;

struct message_t
{
	struct sockaddr_in addr;
	unsigned char* body;
	size_t len;
};

struct conn_t
{
	struct sockaddr_in addr;
	int socket;
};

/* functions */

conn_t* 
conn_init(char address[], port_t port, int flags);

int 
conn_set_timeout(conn_t* con, unsigned long sec);

void
conn_destroy(conn_t* con);

msg_t* 
msg_init(const conn_t* con, const size_t size);

ssize_t 
msg_send(const conn_t* con, const msg_t* msg);

msg_t* 
msg_recv(const conn_t* con, const size_t buf_size);

void
msg_destroy(msg_t* msg);

#endif 