#include <string.h>
#include "client.h"
#include "gui_interface.h"

static conn_t* connection;

bool connect_to_serv(char* serv_ip, port_t port)
{
    msg_t* msg;
    message_type_t m_type = text;


    if(!(connection = conn_init(serv_ip, port, NOFLAGS)))
        return false;

    conn_set_timeout(connection, TIMEOUT);

    if(!(msg = msg_init(connection, sizeof(INIT_MESSAGE) + sizeof(message_type_t))))
        return false;

    msg->len = sizeof(char) * strlen(INIT_MESSAGE) + sizeof(message_type_t);
    memcpy(msg->body, &m_type, sizeof(message_type_t));
    memcpy(msg->body + sizeof(message_type_t), INIT_MESSAGE, sizeof(INIT_MESSAGE));

    msg_send(connection, msg);
    msg_destroy(msg);

    if(!(msg = msg_recv(connection, sizeof(INIT_MESSAGE) + sizeof(message_type_t))))
        return false;

    msg_destroy(msg);

    return true;
}

int main(int argc, char* argv[])
{
    start_gui(argc, argv);

    return 0;
}