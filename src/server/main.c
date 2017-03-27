#include <getopt.h>
#include <signal.h>

#include <query_processing.h>
#include <game_functions.h>
#include <threadpool.h>
#include <job_queue.h>
#include <game_main.h>

#include "config.h"
#include "crypto.h"
#include "network.h"
#include "queue.h"
#include "server.h"
#include "threads.h"
#include "player.h"

static bool is_stopped = false;

static void wait_signal()
{
	sigset_t set;
	siginfo_t info;

	sigemptyset(&set);

	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGINT);

	sigprocmask(SIG_BLOCK, &set, NULL);
	sigwaitinfo(&set, &info);

	is_stopped = true;
	printf("\nStopping server\n");
}

/* set up default values here */ 

static cfg_t* config_default()
{
	cfg_t* cfg = config_init(MAX_OPTS);

	config_setopt(cfg, "max_players", "100");
	config_setopt(cfg, "server_port", "27015");
	config_setopt(cfg, "shard_enabled", "false");
	config_setopt(cfg, "map", "ver1.map");
	config_setopt(cfg, "timeout", "100");

	config_save("server.conf", cfg);
	return cfg;
}

int main(int argc, char* argv[])
{
	/* 
	 *
	 * variables
	 *  
	 */

	char* map_name;
	int opt;
	char* config_file = "server.conf";

	cfg_t* cfg;
	conn_t* con;
	char* err;

	pthread_t input;
	pthread_t output;
	pthread_t game;
	pthread_t server;

	threadpool_t* handler;
	jqueue_t* in;
	queue_t* out;
	players_t* players;

	size_t max_players;
	size_t timeout;
	unsigned short server_port;

	/* 
	 *
	 * cli arguments parsing
	 *  
	 */
	while ((opt = getopt(argc, argv, "hc:m:")) != -1)
	{
		switch (opt)
		{
			case 'c': 
				config_file = optarg; 
				break;
			case 'h':
			default:
				printf("Usage: %s [-f] [config_file]\n", argv[0]);
				return 0;
				break;
		}
	}

	/* 
	 *
	 * load configuration
	 *  
	 */
	cfg = config_load(config_file);
	if (cfg)
	{
		fprintf(stdout, "Loaded configuration from %s\n", config_file);
	}
	else
	{
		fprintf(stderr, "Cannot open file %s.\n", config_file);
		cfg = config_default();
	}

	/* 
	 *
	 * parse configuration
	 *  
	 */
	max_players = atoi(config_getopt(cfg, "max_players"));
	server_port = atoi(config_getopt(cfg, "server_port"));
	timeout = atoi(config_getopt(cfg, "timeout"));
	map_name = config_getopt(cfg, "map");

	if (!game_init(map_name))
	{
		fprintf(stderr, "Cannot load map %s\n", map_name);
		return EXIT_ERROR;
	}

	fprintf(stdout, "Loaded map from %s\n", map_name);

	/* 
	 *
	 * initialize connection
	 *  
	 */
	con = conn_init("0.0.0.0", server_port, BIND);
	if (!con)
	{
		fprintf(stderr, "Cannot connect to %d port\n", server_port);
		return EXIT_ERROR;
	}
	else
	{
		fprintf(stdout, "Server started at port %d \n", server_port);
	}

	/* 
	 *
	 * initialize some modules
	 *  
	 */
	gfunc_init(&err);
	crypto_init();

	/* game handler */
	handler = threadpool_create(2);
	in = threadpool_get_jqueue(handler);
	out = queue_init();
	players = players_init(max_players);

	/* set out queue */
	query_processing_set_out_queue(out);

	/* running I/O server threads */
	run_server_thread(&server, in, &is_stopped, timeout, players);
	run_game_thread(&game, &is_stopped);
	run_output_thread(&output, out, con);
	run_input_thread(&input, in, out, con, players);

    /* waiting for stop */
    wait_signal();

    /* stop threads */
    pthread_cancel(output);
    pthread_cancel(input);
    pthread_cancel(game);

	/* free resources */
	gfunc_destroy();
	threadpool_destroy(handler);
	conn_destroy(con);
	queue_destroy(out, NULL);
	config_destroy(cfg);
	return 0;
}