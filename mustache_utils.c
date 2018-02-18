/* MIT License Copyright 2018  Sebastien Serre */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "mustache_int.h"


/*******************************************************************************
 * Utilities
 ******************************************************************************/
unsigned long
djb2_hash(const char *str)
{
    unsigned long hash = 5381;

    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (unsigned long) hash;
}


/*******************************************************************************
 * Arena memory management
 ******************************************************************************/
struct Arena {
    char *mem;
    char *available;
    char *limit;
    Arena *next;
};


Arena*
Arena_new(const unsigned int base_size)
{
    char *mem = NULL;
    mem = malloc(base_size);
    assert(mem);

    Arena *arena = (Arena*) mem;
    arena->mem = mem;
    arena->available  = mem + sizeof(Arena);
    arena->limit = mem + base_size;
    arena->next = NULL;
    return arena;
}


void*
Arena_malloc(Arena *arena, const unsigned int size)
{
    char* res;
    if ((arena->available + size) < arena->limit) {
        res = arena->available;
        arena->available += size;
        return res;
    } else {
        if (!arena->next) {
            arena->next = Arena_new(arena->limit - arena->mem + size);
        }
        return Arena_malloc(arena->next, size);
    }
}


void*
Arena_calloc(Arena *arena, const unsigned int size)
{
    char *res = Arena_malloc(arena, size);
    memset((void*) res, 0, size);
    return res;
}


void*
Arena_realloc(Arena *arena, void const *ptr,
                    const unsigned int old_size,
                    const unsigned int size)
{
    char *res = Arena_malloc(arena, size);
    if (ptr)
        memcpy(res, ptr, old_size);
    return res;
}


void
Arena_free(Arena *arena)
{
    if (arena->next)
        Arena_free(arena->next);

    free(arena->mem);
}


/*******************************************************************************
 * Dict
 ******************************************************************************/
#define ARENA_BASE_SIZE 65536 /* should old an entire dictionary */
#define DICT_MAX_SIZE 200     /* can old more, but there will be collisions */
#define SECTION_LIST_BASE_SIZE 20

static char empty[] = "";

/* forward declarations */
static Dict* dict_new(Arena*);
static char* dict_search_value(DictEntry*, const char*);
static Dict* dict_get_next_section_element(Dict*, DictEntry*);
static DictEntry* dict_create_entry(Arena*, const char*, char*);
static DictEntry* dict_create_section(Arena*, const char*);
static DictEntry* dict_create_bool_entry(Arena*, const char*, const bool);
void dict_set_value(Dict *dict, const KeyHash *key, char *value);


/* interface */
KeyHash
Mstc_dict_genKeyHash(char *key) {
    KeyHash h;
    h.str = key;
    h.hash = djb2_hash(h.str) % DICT_MAX_SIZE;
    return h;
}


Dict*
Mstc_dict_new()
{
    Arena *arena = Arena_new(ARENA_BASE_SIZE);
    Dict *d = dict_new(arena);
    return d;
}

int
Mstc_dict_setFValue(
    Dict *dict,
    char *key,
    const char *format, ...)
{
    char *value;
    va_list va;
    int nchars;
    KeyHash h;

    va_start(va, format);
    nchars = vsnprintf(NULL, 0, format, va);
    va_end(va);

    nchars++;
    value = Arena_malloc(dict->arena, nchars * 2);

    va_start(va, format);
    vsnprintf(value, nchars * 2, format, va);
    va_end(va);

    h.str = key;
    h.hash = djb2_hash(key) % DICT_MAX_SIZE;

    dict_set_value(dict, &h, value);
    return nchars;
}

void
Mstc_dict_setValue(
    Dict *dict,
    char *key,
    const char *value)
{
    KeyHash h;
    h.str = key;
    h.hash = djb2_hash(key) % DICT_MAX_SIZE;
    char *v = Arena_malloc(dict->arena, strlen(value) + 1);

    strcpy(v, value);
    dict_set_value(dict, &h, v);
}

void
dict_set_value(
    Dict *dict,
    const KeyHash *key,
    char *value)
{
    DictEntry   *entry;
    DictEntry **pentry = &dict->entries[key->hash];

    if (!(*pentry)) {

        *pentry = dict_create_entry(dict->arena, key->str, value);

    } else {

        entry = *pentry;
        while(1) {

            if (
                    (strcmp(entry->key, key->str) == 0) &&
                    (entry->type == STR_ENTRY))
            {
                /* update */

                entry->value = value;
                break;

            } else if (!entry->next) {
                /* append new entry */

                entry->next = dict_create_entry(dict->arena, key->str, value);
                break;

            } else {
                /* continue search */

                entry = entry->next;
                continue;

            }
        }
    }
}


char*
Mstc_dict_getValue(
    const Dict *dict,
    char *key)
{
    KeyHash h;
    h.str = key;
    h.hash = djb2_hash(key) % DICT_MAX_SIZE;
    return Mstc_dict_getValue2(dict, &h);
}


char*
Mstc_dict_getValue2(
    const Dict *dict,
    const KeyHash *key)
{
    char* ret = dict_search_value(dict->entries[key->hash], key->str);

    if ((!strlen(ret)) && dict->parent)
        return Mstc_dict_getValue2(dict->parent, key);

    return ret;
}


Dict*
Mstc_dict_addSectionItem(
    Dict *dict,
    char *key)
{
    KeyHash h;
    h.hash = djb2_hash(key) % DICT_MAX_SIZE;
    h.str = key;
    return Mstc_dict_addSectionItem2(dict, &h);
}


Dict*
Mstc_dict_addSectionItem2(
    Dict *dict,
    const KeyHash *key)
{
    DictEntry *entry;
    DictEntry **pentry = &dict->entries[key->hash];

    if (!(*pentry)) {
        *pentry = dict_create_section(dict->arena, key->str);
        return dict_get_next_section_element(dict, *pentry);
    } else {

        entry = *pentry;
        while(1) {

            if (
                    (strcmp(entry->key, key->str) == 0) &&
                    (entry->type == SECTION_ENTRY))
            {
                /* return new dict */

                return dict_get_next_section_element(dict, entry);

            } else if (!entry->next) {
                /* append new entry and return new dict */

                entry->next = dict_create_section(dict->arena, key->str);
                return dict_get_next_section_element(dict, entry->next);

            } else {
                /* continue search */

                entry = entry->next;
                continue;

            }
        }
    }
}


Dict**
Mstc_dict_getSection(
    const Dict *dict,
    char *key,
    int *nelem)
{
    KeyHash h;
    h.hash = djb2_hash(key) % DICT_MAX_SIZE;
    h.str = key;
    return Mstc_dict_getSection2(dict, &h, nelem);
}


Dict**
Mstc_dict_getSection2(
    const Dict *dict,
    const KeyHash *key,
    int *nelem)
{
    DictEntry *entry = dict->entries[key->hash];

    if (!entry) {
        *nelem = 0;
        return NULL;
    } else {
        while (1) {
            if (
                    (strcmp(entry->key, key->str) == 0) &&
                    (entry->type == SECTION_ENTRY))
            {
                /* found */

                SectionElems *elems = (SectionElems*) entry->value;
                *nelem = elems->nelems;
                return elems->elems;

            } else if (!entry->next) {
                /* no match */

                *nelem = 0;
                return NULL;

            } else {
                /* continue search */

                entry = entry->next;
                continue;

            }
        }
    }
}


void
Mstc_dict_setShowSection(
    Dict *dict,
    char *key,
    const bool show)
{
    KeyHash h;
    h.hash = djb2_hash(key) % DICT_MAX_SIZE;
    h.str = key;
    Mstc_dict_setShowSection2(dict, &h, show);
}


void
Mstc_dict_setShowSection2(
    Dict *dict,
    const KeyHash *key,
    const bool show)
{
    DictEntry   *entry;
    DictEntry **pentry = &dict->entries[key->hash];

    if (!(*pentry)) {

        *pentry = dict_create_bool_entry(dict->arena, key->str, show);

    } else {

        entry = *pentry;
        while(1) {

            if (
                    (strcmp(entry->key, key->str) == 0) &&
                    (entry->type == BOOL_SECTION_ENTRY))
            {
                /* update */
                *((bool*) entry->value) = (bool) show;

                break;

            } else if (!entry->next) {
                /* append new entry */

                entry->next = dict_create_bool_entry(dict->arena, key->str, show);
                break;

            } else {
                /* continue search */

                entry = entry->next;
                continue;

            }
        }
    }
}


bool
Mstc_dict_getShowSection(
    const Dict *dict,
    char *key)
{
    KeyHash h;
    h.str = key;
    h.hash = djb2_hash(key) % DICT_MAX_SIZE;
    return Mstc_dict_getShowSection2(dict, &h);
}


bool
Mstc_dict_getShowSection2(
    const Dict *dict,
    const KeyHash *key)
{
    DictEntry *entry = dict->entries[key->hash];
    bool section_has_data = false;
    while(entry) {

      if (strcmp(entry->key, key->str) == 0)
      {
          if (entry->type == BOOL_SECTION_ENTRY)
          { /* no need to go further */

               return *((bool*) entry->value);

          } else if (entry->type == SECTION_ENTRY) {

            if (((SectionElems*) entry->value)->nelems > 0)
                section_has_data = true;
            else
                section_has_data = false;

          } else { /* TEXT_ENTRY */

                section_has_data = true;

          }
       }

       entry = entry->next;

    }
    return section_has_data;
}


void
Mstc_dict_free(Dict *dict)
{
    Arena_free(dict->arena);
}


/* static functions */
static Dict*
dict_new(Arena *arena)
{
    Dict *d = Arena_malloc(arena, sizeof(Dict));
    d->arena = arena;
    d->parent = NULL;
    d->entries =
        (DictEntry**) Arena_calloc(arena, sizeof(void*) * DICT_MAX_SIZE);

    return d;
}


static char*
dict_search_value(
    DictEntry *entry,
    const char *key)
{
    if (!entry)
        return empty;
    else if (entry->type != STR_ENTRY)
        return dict_search_value(entry->next, key);
    else if (strcmp(entry->key, key) != 0)
        return dict_search_value(entry->next, key);
    else
        return (char*) entry->value;
}


static Dict*
dict_get_next_section_element(
    Dict *dict,
    DictEntry *entry)
{
    SectionElems *e = (SectionElems*) entry->value;
    if (e->limit == e->nelems) {
        unsigned int s = sizeof(Dict*) * e->limit;
        e->elems = (Dict**) Arena_realloc(dict->arena, e->elems, s, s * 3);
        e->limit *= 3;
    }

    Dict *d = dict_new(dict->arena);
    d->parent = dict;
    e->elems[e->nelems] = d;
    e->nelems++;
    return d;
}


static DictEntry*
dict_create_entry(
    Arena *arena,
    const char *key,
    char *value)
{
    DictEntry *entry = Arena_malloc(arena, sizeof(DictEntry));
    entry->next = NULL;
    entry->type = STR_ENTRY;
    entry->key = Arena_malloc(arena, strlen(key) + 1);
    entry->value = value;
    strcpy(entry->key, key);
    return entry;
}


static DictEntry*
dict_create_section(
    Arena *arena,
    const char *key)
{
    DictEntry *entry = Arena_malloc(arena, sizeof(DictEntry));
    entry->next = NULL;
    entry->type = SECTION_ENTRY;
    entry->key = Arena_malloc(arena, strlen(key) + 1);
    strcpy(entry->key, key);

    SectionElems *elems = Arena_malloc(arena, sizeof(SectionElems));;
    elems->nelems = 0;
    elems->elems = Arena_malloc(arena, sizeof(Dict*) * SECTION_LIST_BASE_SIZE);
    elems->limit = SECTION_LIST_BASE_SIZE;
    entry->value = (void*) elems;
    return entry;
}

static DictEntry*
dict_create_bool_entry(
    Arena *arena,
    const char *key,
    const bool show)
{
    DictEntry *entry = Arena_malloc(arena, sizeof(DictEntry));
    entry->next = NULL;
    entry->type = BOOL_SECTION_ENTRY;
    entry->key = Arena_malloc(arena, strlen(key) + 1);
    strcpy(entry->key, key);
    entry->value = Arena_malloc(arena, sizeof(bool));
    *((bool*) entry->value) = (bool) show;
    return entry;
}
