#include <string.h>
#include <malloc.h>
#include <netdb.h>
#include <unistd.h>
#include "db.h"

db_con_t* db_open(char* address, unsigned short port)
{
	struct hostent* h;
	db_con_t* con = malloc(sizeof(db_con_t));
	
	if (!con)
	{
		return NULL;
	}

	if ((con->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		free(con);
		return NULL;
	}

	memset(&con->addr, 0, sizeof(con->addr));
	con->addr.sin_family = AF_INET;
	con->addr.sin_port = htons(port);

	if (!con)
	{
		return NULL;
	}

	h = gethostbyname(address);
	if (!h)
	{
		close(con->socket);
		free(con);
		return NULL;
	}

	con->addr.sin_addr = *(struct in_addr*) h->h_addr_list[0];
	connect(con->socket, (struct sockaddr*) &con->addr, sizeof(con->addr));
	
	return con;
}

void db_close(db_con_t* con)
{
	close(con->socket);
	free(con);
}


char*
db_recv(db_con_t* con)
{
	char buffer[MAX_LEN];
	char* data;
	ssize_t received;
	size_t key_len, data_len;

	received = recvfrom(
		con->socket, 
		buffer, 
		MAX_LEN,
		0,
		NULL, 
		NULL
	);

	if (received < 0)
	{
		return NULL;
	}

	key_len = strlen(buffer);
	data_len = strlen(buffer + key_len + 2) + 1;
	data = malloc (data_len + 1);

	memcpy(data, buffer + key_len + 1, data_len);
	data[data_len] = '\0';
	return data;
}

char* 
db_query(db_con_t* con, char cmd, char* key, char* value)
{
	size_t 
		key_len = 0,
		total_len = 2,
		data_len = 0;
	char req[MAX_LEN];

	key_len = strlen(key);
	total_len += key_len;

	memcpy(&req[1], key, key_len);
	req[0] = cmd;
	req[key_len + 1] = '\0';

	if (value != NULL)
	{
		data_len = strlen(value);
		total_len += data_len;
		memcpy(req + key_len + 2, value, data_len + 1);
	}

	req[total_len] = '\0';
	sendto(
		con->socket,
		req, 
		total_len, 
		0, 
		(struct sockaddr*) &con->addr, 
		sizeof(struct sockaddr)
	);

	return db_recv(con);
}

char* db_set(db_con_t* con, char* key, char* value)
{
	return db_query(con, 'S', key, value);
}

char* db_get(db_con_t* con, char* key)
{
	return db_query(con, 'G', key, NULL);
}

char* db_del(db_con_t* con, char* key)
{
	return db_query(con, 'D', key, NULL);
}