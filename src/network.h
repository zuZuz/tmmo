#ifndef _NETWORK_H_
#define _NETWORK_H_

/* includes */

#include <arpa/inet.h>

/* constants */

#define MAX_LEN 256

/* own types */

typedef struct message_t msg_t;
typedef struct conn_t conn_t;

struct message_t
{
	struct sockaddr_in addr;
	char* body;
	size_t len;
};

struct conn_t
{
	struct sockaddr_in addr;
	int socket;
};

/* functions */

conn_t* 
server_init(unsigned short port);

conn_t* 
client_init(char address[], unsigned short port);

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