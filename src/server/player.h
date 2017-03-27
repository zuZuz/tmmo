#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <netinet/in.h>
#include "network.h"

typedef struct player_t player_t;
typedef struct players_t players_t;

struct player_t
{
	char token[TOKEN_LEN];
	unsigned long timestamp;
	struct sockaddr_in addr;
};

struct players_t
{
	player_t** p;
	size_t max;
	size_t count;
};

players_t* 
players_init(size_t max_players);

void 
players_destroy(players_t* players);

player_t*
player_init(char* token, struct sockaddr_in addr);

bool player_add(players_t* players, char* token, struct sockaddr_in addr);

bool player_delete(players_t* players, size_t pos);

bool player_update(players_t* players, char* token);


#endif