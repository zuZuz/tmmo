#include <string.h>

#include "array.h"

/*
 *
 *	Functions and structs for 
 *	dynamic generation of char[] arrays
 *
 */

static size_t count_symbols(const char* buf, const char c)
{
	size_t count = 0;
	char cur, last = c;

	while ((cur = *buf) != '\0')
	{
		if (cur == c && last != c)
		{
			count++;
		}

		last = *buf;
		buf++;
	}

	if (last != c)
	{
		count++;
	}

	return count;
}

array_t* array_init(const size_t count)
{
	array_t* array = malloc(sizeof(array_t));
	if (!array)
	{
		return NULL;
	}

	array->tokens = malloc(sizeof(char*) * count);
	if (!array->tokens)
	{
		free(array);
		return NULL;
	}

	array->count = 0;
	return array;
}

array_t* array_fill(char* buf)
{
	size_t i, count;
	array_t* array;

	count = count_symbols(buf, ' ');
	array = array_init(count);

	array->tokens[0] = strtok(buf, " ");

	for (i = 1; i < count; i++)
	{
		array->tokens[i] = strtok(NULL, " ");
	}

	return array;
}

void array_destroy(array_t* array)
{
	size_t i;

	for (i = 0; i < array->count; i++)
	{
		free(array->tokens[i]);
	}

	free(array->tokens);
	free(array);
}