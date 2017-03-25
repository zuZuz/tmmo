#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <stdbool.h>
#include <stdio.h>
#include "network.h"

void 
crypto_init();

bool
crypto_key_is_empty(char key[]);

void 
crypto_gen_key(char key[KEY_LEN], size_t len);

void 
crypto_encrypt(char str[], char key[]);

void 
crypto_decrypt(char str[], char key[]);

#endif