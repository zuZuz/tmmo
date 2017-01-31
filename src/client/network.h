#ifndef _NETWORK_H_
#define _NETWORK_H_

/* includes */

#include <arpa/inet.h>

/* constants */

#define MAX_LEN 1024
#define KEY_LEN 32

/* own types */

enum flags
{
    NOFLAGS = 0,
    BIND = 1,

};

enum msg_type_t
{
    conn_test = 0,
    text,
    test = 1668901729,
};

typedef enum msg_type_t msg_type_t;
typedef unsigned short port_t;
typedef struct msg_t msg_t;
typedef struct conn_t conn_t;

struct msg_t
{
    struct sockaddr_in addr;
    msg_type_t type;
    size_t len;
    char key[KEY_LEN];
    char body[MAX_LEN];
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
msg_init(const conn_t* con, char key[KEY_LEN]);

ssize_t
msg_send(const conn_t* con, const msg_t* msg);

msg_t*
msg_recv(const conn_t* con);

void
msg_destroy(msg_t* msg);

#endif 