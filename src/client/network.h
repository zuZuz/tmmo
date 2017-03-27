#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <arpa/inet.h>

#define BODY_LEN 1024
#define TOKEN_LEN 33		// 32 + 1 for \0

enum flags
{
	NOFLAGS = 0,
	BIND = 1,

};

//enum msg_type_t
//{
//    conn_test = 0,
//    text,
//};

enum msg_type_t
{
    conn_test = 0,
    user_msg,
    main_msg,
    chat_msg,
    online_list,
    map_update,
    char_info
};

typedef enum msg_type_t msg_type_t;
typedef unsigned short port_t;
typedef struct msg_t msg_t;
typedef struct conn_t conn_t;

struct msg_t
{
	struct sockaddr_in addr;
	msg_type_t type;
	char key[TOKEN_LEN];
	size_t len;
	char body[BODY_LEN];
};

struct conn_t
{
	struct sockaddr_in addr;
	int socket;
};

conn_t* 
conn_init(char address[], port_t port, int flags);

int 
conn_set_timeout(conn_t* con, unsigned long sec);

void
conn_destroy(conn_t* con);

msg_t* 
msg_init(const conn_t* con);

void 
msg_set_key(msg_t* msg, const char* key);

void 
msg_set_type(msg_t* msg, msg_type_t type);

void 
msg_set_body(msg_t* msg, const char* body);

ssize_t 
msg_send(const conn_t* con, msg_t* msg);

msg_t* 
msg_recv(const conn_t* con);

void
msg_destroy(msg_t* msg);

#endif 