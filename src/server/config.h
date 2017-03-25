#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <stdlib.h>

#include "server.h"

#define MAX_OPTS 32

/* config structures */

typedef struct cfg_t cfg_t;
typedef struct cfg_opt_t cfg_opt_t;

struct cfg_opt_t
{
	char* key;
	char* value;
	cfg_opt_t* next;
};

struct cfg_t
{
	size_t size;
	unsigned mask;
	cfg_opt_t** opts;
	cfg_opt_t* first;
	ssize_t lastopt;
};


cfg_t* 
config_init(size_t size);

cfg_t* 
config_load(char filename[]);

void 
config_save(char filename[], cfg_t* cfg);

void 
config_destroy(cfg_t* cfg);


cfg_opt_t* 
config_opt_init(char* key, char* value);

void 
config_setopt(cfg_t* cfg, char* key, char* value);

char* 
config_getopt(cfg_t* cfg, char* key);

void 
config_opt_destroy(cfg_opt_t* opt);


#endif