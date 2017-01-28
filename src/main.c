#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include "config.h"
#include "network.h"
#include "server.h"
#include "queue.h"

cfg_t* config_default()
{
	cfg_t* cfg = config_init(MAX_OPTS);

	config_setopt(cfg, "max_players", "100");
	config_setopt(cfg, "server_port", "11000");
	config_setopt(cfg, "shard_enabled", "false");

	config_save("server.conf", cfg);
	return cfg;
}

int main(int argc, char* argv[])
{
	/* arguments parsing */
	int opt;
	char* config_file;

	/* variables */
	cfg_t* cfg;
	conn_t* con;

	/* network */
	size_t max_players = 100;
	unsigned short server_port = 11000;


	/* shard */
	bool shard_enabled = false;
	bool is_master;
	char* master_ip;
	unsigned short master_port;
	char* sekret_key;

	/* cli arguments parsing */

	while ((opt = getopt(argc, argv, "f:d")) != -1)
	{
		switch (opt)
		{
			case 'f': 
				config_file = optarg; 
				break;
			default:
				printf(
					"Usage: %s [-f] [config_file]\n", 
					argv[0]
				);
				break;
		}
	}

	/* config load */

	if (config_file)
	{
		cfg = config_load(config_file);
	}
	else
	{
		cfg = config_load("server.conf");
	}

	if (!cfg)
	{
		cfg = config_default();
		fprintf(stderr, "Cannot open configuration file.\n");
	}

	/* config parsing */

	max_players = atoi(config_getopt(cfg, "max_players"));
	server_port = atoi(config_getopt(cfg, "server_port"));
	shard_enabled = atob(config_getopt(cfg, "shard_enabled"));

	if (shard_enabled)
	{
		is_master = atob(config_getopt(cfg, "is_master"));
		master_ip = config_getopt(cfg, "master_ip");
		master_port = atoi(config_getopt(cfg, "master_port"));
		sekret_key = config_getopt(cfg, "sekret_key");
	}

	/* variables initialize */

	if (shard_enabled && !is_master)
	{
		con = conn_init(master_ip, master_port, NOFLAGS);
	}
	else
	{
		con = conn_init("0.0.0.0", server_port, BIND);
	}

	/*
	 *		TODO
	 */

	/* free resources */

	config_destroy(cfg);
	return 0;
}