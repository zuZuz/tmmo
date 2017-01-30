#ifndef _CLIENT_H
#define _CLIENT_H

#define TIMEOUT 3
#define INIT_MESSAGE "init"

#include <stdbool.h>
#include "network.h"

bool connect_to_serv(char* serv_ip, port_t port);

#endif
