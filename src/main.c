#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <string.h>

#include "config.h"
#include "server.h"

cfg_t* confg_default()
{
	cfg_t* cfg = config_init(MAX_OPTS);

	config_setopt(cfg, "max_players", "11000");
	config_setopt(cfg, "port", "11000");
	config_setopt(cfg, "shard_enabled", "false");
	config_setopt(cfg, "port", "11000");

	config_save("server.conf", cfg);
	return cfg;
}

int main(int argc, char* argv[])
{
	return 0;
}