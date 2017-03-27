#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "player.h"
#include "network.h"
#include "queue.h"

/*
 *
 *	online players handling
 *
 */

static unsigned long get_timestamp()
{
	return (unsigned long) time(NULL);
}

player_t* player_init(char* token, struct sockaddr_in addr)
{
	player_t* player = malloc(sizeof(player_t));

	player->timestamp = (unsigned long) time(NULL);
	player->addr = addr;

	memcpy(player->token, token, TOKEN_LEN);
	return player;
}


bool player_add(players_t* players, char* token, struct sockaddr_in addr)
{
	if (players->count == players->max)
	{
		return false;
	}

	printf("added player %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	players->p[players->count++] = player_init(token, addr);
	return true;
}


/* 
 *
 * sooooo ugly. will be rewritten
 *
 */
player_t* player_get(players_t* players, char* token)
{
	size_t i;

	for (i = 0; i < players->count; i++)
	{
		if (!strncmp(token, players->p[i]->token, TOKEN_LEN))
		{
			return players->p[i];
		}
	}

	return NULL;
}

bool player_get_addr(players_t* players, char* token, struct sockaddr_in* addr)
{
	player_t* player = player_get(players, token);

	if (player)
	{
		*addr = player->addr;
		return true;
	}

	return false;
}

bool player_delete(players_t* players, size_t pos)
{
	if (players->count == 0)
	{
		return false;
	}

	printf("deleted player %s:%d\n", inet_ntoa(players->p[pos]->addr.sin_addr), ntohs(players->p[pos]->addr.sin_port));

	free(players->p[pos]);
	memmove(players->p[pos], players->p[pos + 1], players->count - pos - 1);

	players->count--;
	return true;
}

bool player_update(players_t* players, char* token)
{
	player_t* player = player_get(players, token);

	if (player)
	{
		player->timestamp = get_timestamp();
		return true;
	}

	return false;
}

players_t* players_init(size_t max_players)
{
	players_t* players = malloc(sizeof(players_t));
	
	players->p = malloc(sizeof(player_t*) * max_players);
	players->max = max_players;
	players->count = 0;

	return players;
}

void players_destroy(players_t* players)
{
	size_t i;

	for (i = 0; i < players->count; i++)
	{
		free(players->p[i]);
	}

	free(players);
}