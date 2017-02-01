#include "str_hashtable.h"
#include <string.h>


typedef struct str_hashtable_node
{
    void *value;
    uint32_t hashcode;
    struct str_hashtable_node *next;
} str_hashtable_node_t;

typedef struct str_hashtable
{
    str_hashtable_node_t *head;
    size_t size;
} str_hashtable_t;


/*
 * Bob Jenkins's hash function
 */
static uint32_t jenkins_one_at_a_time_hash(char *key, size_t len)
{
    uint32_t hash, i;

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


str_hashtable_t* str_hashtable_create(size_t size)
{
    str_hashtable_t *new_hsht = malloc( sizeof(str_hashtable_t) );

    new_hsht->head = calloc(size, sizeof(str_hashtable_node_t));
    new_hsht->size = size;

    return new_hsht;
}


bool str_hashtable_add(str_hashtable_t *hsht, char *str, void *value)
{
    str_hashtable_node_t *node;
    uint32_t hash;

    if(hsht == NULL  ||  value == NULL  ||  str == NULL)
        return false;

    node = hsht->head;
    hash = jenkins_one_at_a_time_hash(str, strlen(str));

    node +=  hash % hsht->size;

    if(node->value != NULL)
    {
        while(node->hashcode != hash && node->next != NULL)
        {
            node = node->next;
        }

        if(node->hashcode == hash)
            return false;

        node->next = malloc( sizeof(str_hashtable_node_t) );
        node = node->next;
    }

    node->hashcode = hash;
    node->value = value;
    node->next = NULL;

    return true;

}


void* str_hashtable_get(str_hashtable_t *hsht, char *str)
{
    str_hashtable_node_t *node;
    uint32_t hash;

    if(hsht == NULL  ||  str == NULL)
        return NULL;

    node = hsht->head;
    hash = jenkins_one_at_a_time_hash(str, strlen(str));

    node +=  hash % hsht->size;

    while(node != NULL)
    {
        if (node->hashcode == hash)
            return node->value;

        node = node->next;
    }

    return NULL;

}
