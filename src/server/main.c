#include <getopt.h>
#include <netdb.h>

#include "config.h"
#include "network.h"
#include "../game_processing/query_processing.h"
#include "queue.h"
#include "server.h"
#include "threads.h"
#include "../game_processing/game_functions.h"

cfg_t* config_default()
{
	cfg_t* cfg = config_init(MAX_OPTS);

	config_setopt(cfg, "max_players", "100");
	config_setopt(cfg, "server_port", "27015");
	config_setopt(cfg, "shard_enabled", "false");

	config_save("server.conf", cfg);
	return cfg;
}

int main(int argc, char* argv[])
{
	/* arguments parsing */
	int opt;
	char* config_file = "server.conf";

	/* common */
	cfg_t* cfg;
	conn_t* con;
	queue_t* in_queue;
	queue_t* out_queue;
	char* err;

	/* threads */
	pthread_t receiver;
	pthread_t sender;
	pthread_t processer;

	/* network */
	size_t max_players = 100;
	unsigned short server_port = 27015;

	/* shard */
	bool shard_enabled = false;
	bool is_master;
	char* master_ip;
	unsigned short master_port;
	char* sekret_key;
	
	/* terminating */ 
	bool is_terminated = false;

	/* cli arguments parsing */
	while ((opt = getopt(argc, argv, "f:")) != -1)
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
	cfg = config_load(config_file);
	if (!cfg)
	{
		cfg = config_default();
		fprintf(stderr, "Couldn't open configuration file %s.\n", 
			config_file);
	}
	else
	{
		fprintf(stdout, "Loaded configuration from %s.\n", 
			config_file);
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

	/* variable initialization */
	con = conn_init("0.0.0.0", server_port, BIND);
	if (!con)
	{
		fprintf(stderr, "Couldn't connect to port %d.\n", server_port);
		return EXIT_ERROR;
	}
	else
	{
		fprintf(stdout, "Started at port %d.\n", server_port);
	}

	in_queue = queue_init();
	out_queue = queue_init();

	if (!gfunc_init(&err))
	{
		fprintf(stderr, "%s \n", err);
		return EXIT_ERROR;
	}

	thread_arg recvr_arg = {in_queue, con, &is_terminated};
	thread_arg sender_arg = {out_queue, con, &is_terminated};
	qprocess_args_t pr_arg = {in_queue, out_queue, &is_terminated};

	/* services running */
    pthread_create(&receiver, NULL, receiver_thread, &recvr_arg);
    pthread_create(&sender, NULL, sender_thread, &sender_arg);
    pthread_create(&processer, NULL, query_processing, &pr_arg);

    pthread_join(receiver, NULL);
    pthread_join(sender, NULL);
    pthread_join(processer, NULL);

	/* free resources */
	queue_destroy(in_queue, free);
	queue_destroy(out_queue, free);
	conn_destroy(con);
	config_destroy(cfg);
	return 0;
}