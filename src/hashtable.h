/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2011, Lukas Weber <laochailan@web.de>
 * Copyright (C) 2012, Alexeyew Andrew <http://akari.thebadasschoobs.org/>
 */

#ifndef TSHASHTABLE_H
#define TSHASHTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct Hashtable Hashtable;
typedef uint64_t hash_t;

typedef bool (*HTCmpFunc)(void *key1, void *key2);
typedef hash_t (*HTHashFunc)(void *key);
typedef void (*HTCopyFunc)(void **dstkey, void *srckey);
typedef void (*HTFreeFunc)(void *key);
typedef void* (*HTIterCallback)(void *key, void *data, void *arg);

Hashtable* hashtable_new(size_t size, HTCmpFunc cmp_func, HTHashFunc hash_func, HTCopyFunc copy_func, HTFreeFunc free_func);
void hashtable_free(Hashtable *ht);
void* hashtable_get(Hashtable *ht, void *key);
void hashtable_set(Hashtable *ht, void *key, void *data);
void hashtable_unset(Hashtable *ht, void *key);
void* hashtable_foreach(Hashtable *ht, HTIterCallback callback, void *arg);

bool hashtable_cmpfunc_string(void *str1, void *str2);
hash_t hashtable_hashfunc_string(void *vstr);
void hashtable_copyfunc_string(void **dst, void *src);
#define hashtable_freefunc_string free
Hashtable* hashtable_new_stringkeys(size_t size);

void* hashtable_get_string(Hashtable *ht, const char *key);
void hashtable_set_string(Hashtable *ht, const char *key, void *data);
void hashtable_unset_string(Hashtable *ht, const char *key);

int hashtable_test(void);

#endif
