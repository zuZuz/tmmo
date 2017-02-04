#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "crypto.h"
#include "network.h"

static char sid[] = {
	'0','1','2','3','4',
	'5','6','7','8','9',
	'A','B','C','D','E',
	'F','G','H','I','J',
	'K','L','M','N','O',
	'P','Q','R','S','T',
	'U','V','W','X','Y',
	'Z','a','b','c','d',
	'e','f','g','h','i',
	'j','k','l','m','n',
	'o','p','q','r','s',
	't','u','v','w','x',
	'y','z'
};

/* 
 *
 *	hash-function rot13 and
 *	basic xor crypto function
 *
 */

static unsigned get_hash(char str[])
{
	size_t i;
	unsigned int hash = 0;
	size_t len = strlen(str);

	for(i = 0; i < len; i++)
	{
		hash += str[i];
		hash -= (hash << 13) | (hash >> 19);
	}

	return hash;
}

void crypto_init()
{
	srand(time(NULL));
}

void crypto_gen_key(char key[], size_t len)
{
	size_t i;
	size_t sid_len = strlen(sid);

	for (i = 0; i < len; i++)
	{
		key[i] = sid[rand() % (sid_len - 1)];
	}

	key[len] = 0;
}

bool crypto_key_is_empty(char key[])
{
	return key[0] == '\0';
}

void crypto_encrypt(char str[], char key[])
{
	size_t i, str_len, key_len;
	unsigned m1 = 0, m2 = 0;
	unsigned hash = get_hash(key) & 0xff;

	str_len = strlen(str);
	key_len = strlen(key);

	for (i = 0; i < key_len; i += 2)
	{
		m1 += key[i];
		m1 -= (m1 << 13) | (m1 >> 19);
	}

	for (i = 1; i < key_len; i += 2)
	{
		m2 += key[i];
		m2 -= (m2 << 13) | (m2 >> 19);
	}


	for (i = 0; i < str_len; i++)
	{
		str[i] ^= m1;
		str[i] -= hash;
		str[i] ^= m2;
	}
}

void crypto_decrypt(char str[], char key[])
{
	size_t i, str_len, key_len;
	unsigned m1 = 0, m2 = 0;
	unsigned hash = get_hash(key);

	str_len = strlen(str);
	key_len = strlen(key);

	for (i = 0; i < key_len; i += 2)
	{
		m1 += key[i];
		m1 -= (m1 << 13) | (m1 >> 19);
	}

	for (i = 1; i < key_len; i += 2)
	{
		m2 += key[i];
		m2 -= (m2 << 13) | (m2 >> 19);
	}	

	for (i = 0; i < str_len; i++)
	{
		str[i] ^= m2;
		str[i] += hash;
		str[i] ^= m1;
	}
}
