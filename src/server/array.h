#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <stdlib.h>

typedef struct array_t array_t;

struct array_t
{
	size_t count;
	char** tokens;
};

array_t* 
array_init(const size_t count);

array_t* 
array_fill(char* buf);

void 
array_destroy(array_t* array);

#endif