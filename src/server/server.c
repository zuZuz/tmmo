#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

bool atob(char* str)
{
	if (!strcmp("true", str))
	{
		return true;
	}

	return false;
}