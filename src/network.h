#ifndef _NETWORK_H_
#define _NETWORK_H_

/* includes */

#include <arpa/inet.h>

/* constants */

#define MAX_LEN 256

/* own types */

typedef struct message_t msg_t;
typedef struct conn_t conn_t;

enum status_t
{
	OK = 0,
	SOCKET_ERROR = -1,
	BIND_ERROR = -2,
	RECV_ERROR = -3,
	MALFORMED_ADDR = -4,
};

typedef enum status_t status_t;

struct message_t
{
	struct sockaddr_in addr;
	char* body;
	ssize_t len;
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
msg_init(conn_t* con, size_t size);

ssize_t 
msg_send(const conn_t* con, const msg_t* msg);

msg_t* 
msg_recv(const conn_t* con, const size_t buf_size);

void
msg_destroy(msg_t* msg);

#endif 