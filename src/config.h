#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN 256
#define MAX_OPTS 32

/* opts types */

enum cfg_type_t
{
	INT_PROP = 0,
	BOOL_PROP = 1,
	STR_PROP = 2,
};

typedef enum cfg_type_t cfg_type_t;

/* config structures */

typedef struct cfg_t cfg_t;
typedef struct cfg_opt_t cfg_opt_t;

struct cfg_opt_t
{
	char* key;
	char* value;
};

struct cfg_t
{
	size_t size;
	unsigned mask;
	cfg_opt_t** opts;
};

cfg_t* 
config_load(char filename[]);

char* 
config_getopt(cfg_t* cfg, char* key);

void 
config_destroy(cfg_t* cfg);


#endif