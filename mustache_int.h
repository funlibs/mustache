/* MIT License Copyright 2018  Sebastien Serre
 *
 * This file contains an extra API ovoiding the hash algorithm for each dict
 * call, using a KeyHash structure.
 */


#ifndef _MUSTACHE_INT_H_
#define _MUSTACHE_INT_H_

#include <stdbool.h>

/*******************************************************************************
 *
 * UTILS
 *
 ******************************************************************************/
unsigned long djb2_hash(const char *str);

/*******************************************************************************
 * Arena memory management
 ******************************************************************************/
typedef struct Arena Arena;

Arena* Arena_new(unsigned int const need);
void* Arena_malloc(Arena *arena, unsigned int const size);
void* Arena_calloc(Arena *arena, unsigned int size);
void* Arena_realloc(
    Arena *arena,
    void const *ptr,
    unsigned int const old_size,
    unsigned int const size);
void Arena_free(Arena *arena);


/*******************************************************************************
 * Dict
 ******************************************************************************/
/*
 * Hash algorythm
 */
typedef struct Dict Dict;
typedef struct DictEntry DictEntry;
typedef struct SectionElems SectionElems;
typedef enum {STR_ENTRY,SECTION_ENTRY,BOOL_SECTION_ENTRY} DictEntryType;

struct Dict {
    void *arena;
    Dict *parent;  /* parent context */
    DictEntry **entries;
};

struct DictEntry {
    char *key;
    void *value;        /* can be char*, SectionElems* or *bool */
    DictEntry *next;
    DictEntryType type; /* What's in value */
};

struct SectionElems {
    int nelems;
    int limit;
    Dict **elems;
};

typedef struct KeyHash {
    unsigned int hash;
    char *str;
} KeyHash;


/*
 * Generate hashed key. key must be accessible (allocated) for use.
 * Used with the second versions of the dict api.
 */
KeyHash Mstc_dict_genKeyHash(char *key);

/**
 * Every functions left exist in two versions. The one present in mustache_api
 * use a string as key for the dict and compute the hash at each call.
 * These ones second (sufixed with "2") use the KeyHash struct embeding both
 * the string and the hash. The second should be used if performance is a
 * matter.
 *
 * The get* functions are used by the Mstc_expand function.
 */
Dict* Mstc_dict_addSectionItem2(Dict *dict, const KeyHash *key);

void Mstc_dict_setValue3(Dict *dict, const KeyHash *key, const char *value);
char* Mstc_dict_getValue(const Dict *dict, char *key);
char* Mstc_dict_getValue2(const Dict *dict, const KeyHash *key);
Dict** Mstc_dict_getSection(const Dict *dict, char *key, int *nelem);
Dict** Mstc_dict_getSection2(const Dict *dict, const KeyHash *key, int *nelem);
void Mstc_dict_setShowSection2(Dict *dict, const KeyHash *key, const bool show);
bool Mstc_dict_getShowSection(const Dict *dict, char *key);
bool Mstc_dict_getShowSection2(const Dict *dict, const KeyHash *key);


/*******************************************************************************
 * Loader
 ******************************************************************************/
typedef enum {
    STRING_TOKEN = 0,        /* a simple string */
    KEY_TOKEN = 1,           /* a keyword that have to be replaced */
    KEY_TOKEN_NO_ESCAPE = 2, /* a keyword that have to be replaced (no html escape) */
    SECTION_TOKEN = 3,       /* a group, begining with #bla end ending with /bla */
    INV_SECTION_TOKEN = 4,   /* a group, begining with ^bla end ending with /bla */
    BOOL_SECTION_TOKEN = 5,  /* a group, begining with ?bla end ending with /bla */
    ROOT_SECTION_TOKEN = 6   /* the root element */
} TokenType;

typedef struct StaticString {
    char *str;
    unsigned int len;
} StaticString;

typedef struct Token Token;
struct Token {
    void*     value;     /* char, KeyHash or StaticString */
    TokenType type;
    Token     *childs;
    int       nchilds;
    int       size;
};

typedef struct Template Template;
struct Template {
    char *filename;
    Token root;
    Template *next;
};

/* all tplTemplates loaded */
typedef struct TemplateStore TemplateStore;
struct TemplateStore {
    Arena *arena;
    Template **res;
};

char* Mstc_template_getTypeFromCode(TokenType);
/*
 * For debuging purpose
 */
extern void Mstc_template_printTokenStructure(Template *t);

#endif /* _MUSTACHE_INT_H_ */
