#ifndef _DB_H_
#define _DB_H_

#include <stdbool.h>
#include <arpa/inet.h>

#define KEY_LEN 128
#define DATA_LEN 1024

#define MAX_LEN KEY_LEN + DATA_LEN + 2

struct db_con_t
{
	struct sockaddr_in addr;
	int socket;
};

typedef struct db_con_t db_con_t;

db_con_t*
db_open(char* address, unsigned short port);

void 
db_close(db_con_t* con);

char*
db_query(db_con_t* con, char cmd, char* key, char* value);

char*
db_set(db_con_t* con, char* key, char* value);

char*
db_get(db_con_t* con, char* key);

char*
db_del(db_con_t* con, char* key);

#endif