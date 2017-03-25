#include "client.h"
#include "gui.h"

#include <string.h>

static pthread_t server_worker;
static conn_t *actv_conn;
static bool pthr_is_active;

static char key[KEY_LEN];

static bool check_connection(conn_t* conn)
{
    msg_t* msg;

    msg = msg_init(conn);
    if(msg == NULL) return false;

    msg->type = conn_test;

    msg_send(conn, msg);
    msg_destroy(msg);

    msg = msg_recv(conn);
    if(msg == NULL) return false;

    if(msg->type != conn_test) return false;

    memcpy(key, msg->key, KEY_LEN);
    msg_destroy(msg);

    return true;
}

static void msg_process(msg_t* msg)
{
    if(msg == NULL) return;

    switch (msg->type)
    {
        case text:
            gui_print_main_msg(msg->body);
            break;

        default:
            break;
    }
}

static void* serv_listen_routine(void* args)
{
    msg_t* msg;

    while(pthr_is_active)
    {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

        msg = msg_recv(actv_conn);

        if(msg == NULL) continue;

        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        msg_process(msg);
    }

    return NULL;
}

static bool start_connection()
{
    pthr_is_active = true;
    pthread_create(&server_worker, NULL, serv_listen_routine, NULL);

    return true;
}

void close_connection()
{
    pthr_is_active = false;

    pthread_cancel(server_worker);
    pthread_join(server_worker, NULL);

    conn_destroy(actv_conn);
}

bool connect_to_serv(char* serv_ip, port_t port)
{
    conn_t* check_conn;

    if((check_conn = conn_init(serv_ip, port, NOFLAGS)) == NULL) return false;

    conn_set_timeout(check_conn, CONN_CHECK_TIMEOUT);

    if(!check_connection(check_conn))
    {
        conn_destroy(check_conn);
        return false;
    }

    if(actv_conn != NULL) close_connection();

    actv_conn = check_conn;
    conn_set_timeout(actv_conn, 0);

    start_connection();

    return true;
}

void send_user_input(const char* input, size_t len)
{
    msg_t* msg;

    msg = msg_init(actv_conn);
    if(msg == NULL) return;

    msg->type = text;
    msg->len = len;

    msg_set_body(msg, input);
    msg_set_key(msg, key);

    msg_send(actv_conn, msg);
    msg_destroy(msg);
}

int main(int argc, char* argv[])
{
    return gui_start(argc, argv);
}