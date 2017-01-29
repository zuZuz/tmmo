#include <string.h>
#include "client.h"
#include "gui.h"

static conn_t* connection;

bool connect_to_serv(char* serv_ip, port_t port)
{
    msg_t* msg;
    message_type_t m_type = text;


    if(!(connection = conn_init(serv_ip, port, NOFLAGS)))
        return false;

    conn_set_timeout(connection, TIMEOUT);

    if(!(msg = msg_init(connection, sizeof(INIT_MESSAGE) + sizeof(m_type))))
        return false;

    memcpy(msg->body, &m_type, sizeof(m_type));
    memcpy(msg->body + sizeof(m_type), INIT_MESSAGE, sizeof(INIT_MESSAGE));

    msg_send(connection, msg);
    msg_destroy(msg);

    if(!(msg = msg_recv(connection, sizeof(INIT_MESSAGE) + sizeof(m_type))))
        return false;

    main_text_message((char*)(msg->body + sizeof(m_type)));
    msg_destroy(msg);

    return true;
}