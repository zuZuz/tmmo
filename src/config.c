#include <stdio.h>
#include <string.h>

#include "config.h"

/*
 *
 *	Config based on simple hash-table.
 *	get_hash - Jenkins hash function
 *  https://en.wikipedia.org/wiki/Jenkins_hash_function
 *
 *	Collisions are not considered.
 *
 */

static unsigned get_hash(char *key)
{
    unsigned hash; 
    size_t i;
    size_t len = strlen(key);

    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return hash;
}

static char* file_get_line(FILE* file, char buffer[], size_t size)
{
	if (!fgets(buffer, size, file))
	{
		return NULL;
	}

	if (buffer[strlen(buffer) - 1] =='\n')
	{	
		buffer[strlen(buffer) - 1] = '\0';
	}

	return buffer;
}

static cfg_t* config_init(size_t size)
{
	cfg_t* cfg = malloc(sizeof(cfg_t));
	if (!cfg)
	{
		return NULL;
	}

	cfg->opts = calloc(size, sizeof(cfg_opt_t));
	if (!cfg->opts)
	{
		free(cfg);
		return NULL;
	}

	cfg->size = size;
	cfg->mask = size - 1;

	return cfg;
}

static cfg_opt_t* config_opt_init(char* key, char* value)
{
	cfg_opt_t* opt = malloc(sizeof(cfg_opt_t));
	if (!opt)
	{
		return NULL;
	}

	opt->key = malloc(strlen(key));
	opt->value = malloc(strlen(value));

	if (!opt->key || !opt->value)
	{
		free(opt);
		return NULL;
	}

	memcpy(opt->key, key, strlen(key));
	memcpy(opt->value, value, strlen(value));

	return opt;
}

void config_setopt(cfg_t* cfg, char* key, char* value)
{
	unsigned pos = get_hash(key) & cfg->mask;
	cfg->opts[pos] = config_opt_init(key, value);
}

char* config_getopt(cfg_t* cfg, char* key)
{
	unsigned pos = get_hash(key) & cfg->mask;
	return cfg->opts[pos]->value;
}

cfg_t* config_load(char filename[])
{
	char *key, *value;
	char buffer[MAX_LEN];
	cfg_t* cfg = config_init(MAX_OPTS);
	FILE* file = fopen(filename, "r");

	while (!feof(file))
	{
		file_get_line(file, buffer, MAX_LEN);

		key = strtok(buffer, " =");
		value = strtok(NULL, " =");

		if (key && value)
		{
			config_setopt(cfg, key, value);
		}
	}

	fclose(file);
	return cfg;
}

void config_save(char filename[], cfg_t* cfg)
{
	size_t i;
	FILE* file = fopen(filename, "w");

	for (i = 0; i < cfg->size; i++)
	{
		if (cfg->opts[i])
		{
			fprintf(
				file, "%s = %s\n", 
				cfg->opts[i]->key, 
				cfg->opts[i]->value
			);
		}
	}
}

void config_opt_destroy(cfg_opt_t* opt)
{
	free(opt);
}

void config_destroy(cfg_t* cfg)
{
	size_t i;

	for (i = 0; i < cfg->size; i++)
	{
		if (cfg->opts[i])
		{
			config_opt_destroy(cfg->opts[i]);		
		}
	}

	free(cfg);
}