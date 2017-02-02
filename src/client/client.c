#include "client.h"
#include "gui_interface.h"
#include "msg_queue.h"

#include <string.h>

static pthread_t pthr_serv_listener, pthr_msg_processor;
static msg_queue_t *msg_queue;
static conn_t *actv_conn;
static bool pthr_is_active;

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

    if(msg->type != conn_test)
        return false;

    msg_destroy(msg);

    return true;
}

static void msg_process(msg_t* msg)
{
    if(msg == NULL)
        return;

    switch (msg->type)
    {
        case text:
            gui_print_main_msg(msg->body);
            break;

        default:
            break;
    }
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
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

        msg = msg_recv(actv_conn);

        if(msg == NULL)
            continue;

        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        msg_queue_enqueue(msg_queue, msg);
    }

    return NULL;
}

/*init queue and run threads*/
static bool start_connection()
{
    msg_queue = msg_queue_init();
    if(msg_queue == NULL)
        return false;

    pthr_is_active = true;

    pthread_create(&pthr_serv_listener, NULL, serv_listen_routine, NULL);
    pthread_create(&pthr_msg_processor, NULL, msg_processor_routine, NULL);

    return true;
}

void close_connection()
{
    pthr_is_active = false;

    pthread_cond_signal(&msg_queue->cond);

    pthread_cancel(pthr_serv_listener);

    pthread_join(pthr_serv_listener, NULL);
    pthread_join(pthr_msg_processor, NULL);

    conn_destroy(actv_conn);

    msg_queue_destroy(msg_queue);
}

bool connect_to_serv(char* serv_ip, port_t port)
{
    conn_t* check_conn;

    if((check_conn = conn_init(serv_ip, port, NOFLAGS)) == NULL)
        return false;

    conn_set_timeout(check_conn, CONN_CHECK_TIMEOUT);

    if(!check_connection(check_conn))
    {
        conn_destroy(check_conn);
        return false;
    }

    if(actv_conn != NULL)
        close_connection();

    actv_conn = check_conn;
    conn_set_timeout(actv_conn, 0);

    start_connection();

    return true;
}

void send_user_input(const char* input, size_t len)
{
    msg_t* msg;

    msg = msg_init(actv_conn, NULL);
    if(msg == NULL)
        return;

    msg->type = text;
    msg->len = len;

    memcpy(msg->body, input, msg->len);

    msg_send(actv_conn, msg);
    msg_destroy(msg);
}

int main(int argc, char* argv[])
{
    gui_start(argc, argv);

    return 0;
}