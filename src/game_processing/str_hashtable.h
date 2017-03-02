/*
 * Hash table for storing strings
 * All strings must have a different hash code (otherwise an error when adding)
 */

#include <stdlib.h>
#include <stdbool.h>

#ifndef STR_HASHTABLE_H
#define STR_HASHTABLE_H

typedef struct str_hashtable str_hashtable_t;

str_hashtable_t* str_hashtable_create(size_t size);

/*
 * return the hashcode in str_hashcode
 */
bool str_hashtable_add(str_hashtable_t *hsht, char *str, void *value, u_int32_t *str_hashcode);

void* str_hashtable_get(str_hashtable_t *hsht, char *str);
void str_hashtable_destroy(str_hashtable_t *hsht);

#endif //STR_HASHTABLE_H