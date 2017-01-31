#include "client.h"
#include "gui_interface.h"
#include "msg_queue.h"

#include <stdlib.h>
#include <string.h>

static pthread_t *pthr_serv_listener, *pthr_msg_processor;
static msg_queue_t *msg_queue;
static bool pthr_is_active;
static conn_t *connection;

static bool check_connection(conn_t* conn)
{
    msg_t* msg;

    msg = msg_init(conn, NULL);
    if(msg == NULL)
        return false;

    msg->type = conn_test;

    msg_send(conn, msg);
    msg_destroy(msg);

    msg = msg_recv(conn);
    if(msg == NULL)
        return false;

    msg_destroy(msg);

    return true;
}

/*TODO: message processor */
static void msg_process(msg_t* msg)
{
    if(msg == NULL)
        return;
}

static void* msg_processor_routine(void* args)
{
    while(pthr_is_active)
    {
        pthread_mutex_lock(&msg_queue->mutex);

        while (msg_queue_is_empty(msg_queue) && pthr_is_active) {
            pthread_cond_wait( &(msg_queue->cond), &(msg_queue->mutex) );
        }

        pthread_mutex_unlock(&msg_queue->mutex);

        msg_process(msg_queue_dequeue(msg_queue));
    }

    return NULL;
}

static void* serv_listen_routine(void* args)
{
    msg_t* msg;

    while(pthr_is_active)
    {
        msg = msg_recv(connection);

        if(msg == NULL)
            continue;

        msg_queue_enqueue(msg_queue, msg);
    }

    return NULL;
}

static bool start_connection()
{
    msg_queue = msg_queue_init();
    if(msg_queue == NULL)
        return false;

    pthr_is_active = true;

    pthread_create(pthr_serv_listener, NULL, serv_listen_routine, NULL);
    pthread_create(pthr_msg_processor, NULL, msg_processor_routine, NULL);

    return true;
}

void close_connection()
{
    if(connection == NULL)
        return;

    pthr_is_active = false;
    pthread_cond_signal(&msg_queue->cond);

    pthread_join(*pthr_serv_listener, NULL);
    pthread_join(*pthr_msg_processor, NULL);

    free(pthr_msg_processor);
    free(pthr_serv_listener);

    msg_queue_destroy(msg_queue);

    conn_destroy(connection);
}

bool connect_to_serv(char* serv_ip, port_t port)
{
    conn_t* conn;

    if((conn = conn_init(serv_ip, port, NOFLAGS)) == NULL)
        return false;

    conn_set_timeout(conn, TIMEOUT);

    if(!check_connection(conn))
    {
        conn_destroy(conn);
        return false;
    }

    close_connection();

    connection = conn;

    start_connection();

    return true;
}

void send_user_input(const char* input)
{
    msg_t* msg;

    msg = msg_init(connection, NULL);
    if(msg == NULL)
        return;

    msg->type = text;
    msg->len = strlen(input);

    memcpy(msg->body, input, msg->len);

    msg_send(connection, msg);
    msg_destroy(msg);
}

int main(int argc, char* argv[])
{
    start_gui(argc, argv);

    return 0;
}