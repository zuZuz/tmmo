#include <arpa/inet.h>
#include <malloc.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "network.h"

static conn_t* conn_socket(port_t port)
{
    conn_t* con = malloc(sizeof(conn_t));

    if (!con)
    {
        return NULL;
    }

    if ((con->socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        free(con);
        return NULL;
    }

    memset(&con->addr, 0, sizeof(con->addr));
    con->addr.sin_family = AF_INET;
    con->addr.sin_port = htons(port);

    return con;
}

static conn_t* conn_bind(conn_t* con)
{
    if (bind(con->socket, (struct sockaddr*)
            &con->addr, sizeof(struct sockaddr)) == -1)
    {
        conn_destroy(con);
        return NULL;
    }

    return con;
}

conn_t* conn_init(char address[], port_t port, int flags)
{
    conn_t* con = conn_socket(port);

    if (!con)
    {
        return NULL;
    }

    if (inet_aton(address, &con->addr.sin_addr) == 0)
    {
        conn_destroy(con);
        return NULL;
    }

    if ((flags & BIND) && !conn_bind(con))
    {
        conn_destroy(con);
        return NULL;
    }

    return con;
}

int conn_set_timeout(conn_t* con, unsigned long sec)
{
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = 0;

    return setsockopt(
            con->socket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &timeout, sizeof(timeout)
    );
}

void conn_destroy(conn_t* con)
{
    close(con->socket);
    free(con);
}

msg_t* msg_init(const conn_t* con, char key[KEY_LEN])
{
    msg_t* msg = malloc(sizeof(msg_t));
    if (!msg)
    {
        return NULL;
    }

    msg->len = 0;

    if (con)
    {
        msg->addr = con->addr;
    }

    if (key)
    {
        memcpy(msg->key, key, KEY_LEN);
    }

    return msg;
}

ssize_t msg_send(const conn_t* con, const msg_t* msg)
{
    return sendto(
            con->socket,
            &msg->type,
            sizeof(msg_t) - sizeof(struct sockaddr),
            NOFLAGS,
            (struct sockaddr*) &msg->addr,
            sizeof(struct sockaddr)
    );
}

msg_t* msg_recv(const conn_t* con)
{
    msg_t* msg;
    unsigned size;
    ssize_t received;

    size = sizeof(struct sockaddr);
    msg = msg_init(NULL, NULL);

    received = recvfrom(
            con->socket,
            &msg->type,
            sizeof(msg_t) - sizeof(struct sockaddr),
            NOFLAGS,
            (struct sockaddr*) &msg->addr,
            &size
    );

    if (received < 0)
    {
        msg_destroy(msg);
        return NULL;
    }

    return msg;
}

void msg_destroy(msg_t* msg)
{
    free(msg);
}