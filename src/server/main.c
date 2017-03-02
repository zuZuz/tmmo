#include <getopt.h>
#include <signal.h>

#include "config.h"
#include "crypto.h"
#include "network.h"
#include "queue.h"
#include "server.h"
#include "threads.h"

#include "../game_processing/query_processing.h"
#include "../game_processing/game_functions.h"
#include "../game_processing/threadpool.h"
#include "../game_processing/job_queue.h"

#define WORKERS_COUNT 2

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

	printf("\nStopping server\n");
}

int main(int argc, char* argv[])
{
	/* arguments parsing */
	int opt;
	char* config_file = "server.conf";

	/* common */
	cfg_t* cfg;
	conn_t* con;
	char* err;

	/* threads */
	pthread_t input;
	pthread_t output;

	/* processer */
	threadpool_t* handler;
	jqueue_t* in;
	queue_t* out;

	/* network */
	size_t max_players = 100;
	unsigned short server_port = 27015;

	/* cli arguments parsing */
	while ((opt = getopt(argc, argv, "f:")) != -1)
	{
		switch (opt)
		{
			case 'f': 
				config_file = optarg; 
				break;
			default:
				printf("Usage: %s [-f] [config_file]\n", argv[0]);
				break;
		}
	}

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

	max_players = atoi(config_getopt(cfg, "max_players"));
	server_port = atoi(config_getopt(cfg, "server_port"));

	con = conn_init("0.0.0.0", server_port, BIND);
	if (!con)
	{
		fprintf(stderr, "Cannot connect to %d port\n", server_port);
		return EXIT_ERROR;
	}
	else
	{
		fprintf(stdout, "Started at port %d \n", server_port);
		conn_set_timeout(con, 1);
	}

	gfunc_init(&err);
	crypto_init();

	/* game handler */
	handler = threadpool_create(WORKERS_COUNT);
	in = threadpool_get_jqueue(handler);
	out = threadpool_get_out_queue(handler);

	/* service msg handler */

	/* running I/O server threads */
	run_output_thread(&output, out, con);
	run_input_thread(&input, in, con);

    /* waiting for stop */
    wait_signal();

    /* stop threads */
    pthread_cancel(output);
    pthread_cancel(input);

	/* free resources */
	gfunc_destroy();
	threadpool_destroy(handler);
	conn_destroy(con);
	config_destroy(cfg);
	return 0;
}