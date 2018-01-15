/* MIT License Copyright 2018  Sebastien Serre */

#ifndef _MUSTACHE_H_
#define _MUSTACHE_H_

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
 * Will create a "main" dictionary. This is the one we will want to
 * freed with Mstc_dict_free().
 */
Dict* Mstc_dict_new();

/*
 * Generate hashed key. key must be accessible (allocated) for use.
 */
KeyHash Mstc_dict_genKeyHash(char *key);

/**
 *
 * Every functions left exist in two versions. The first use a string as key
 * for the dict and compute the hash at each call. The second (sufixed with "2")
 * use the KeyHash struct embeding both the string and the hash. The second
 * should be used if performance is a matter.
 *
 */

/*
 * Set and get text values by key string.
 */
void Mstc_dict_setValue(Dict *dict, char *key, const char *value);
void Mstc_dict_setValue2(Dict *dict, const KeyHash *key, const char *value);

char* Mstc_dict_getValue(const Dict *dict, char *key);
char* Mstc_dict_getValue2(const Dict *dict, const KeyHash *key);


/*
 * Create and get sub dictionaries.
 * showSection(false), will hide a simple sections (of the same name) that
 * contains some values and which would have been evaluated to true without
 * this explicit false.
 * showsection(true), will show content of the section.
 */
Dict* Mstc_dict_addSectionItem(Dict *dict, char *key);
Dict* Mstc_dict_addSectionItem2(Dict *dict, const KeyHash *key);

Dict** Mstc_dict_getSection(const Dict *dict, char *key, int *nelem);
Dict** Mstc_dict_getSection2(const Dict *dict, const KeyHash *key, int *nelem);

void Mstc_dict_setShowSection(Dict *dict, char *key, const bool show);
void Mstc_dict_setShowSection2(Dict *dict, const KeyHash *key, const bool show);

bool Mstc_dict_getShowSection(const Dict *dict, char *key);
bool Mstc_dict_getShowSection2(const Dict *dict, const KeyHash *key);

/*
 * Free the main dictionnary.
 */
void Mstc_dict_free(Dict* dict);


/*******************************************************************************
 *
 * API
 *
 ******************************************************************************/

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
    ROOT_SECTION_TOKEN = 6,  /* the root element */
} TokenType;

typedef struct StaticString {
    char *str;
    int len;
} StaticString;

typedef struct Token Token;
struct Token {
    void*     value;     /* char, KeyHash or StaticString */
    TokenType type;
    Token     *childs;
    int       nchilds;
    int       size;
};

typedef struct Ressource Ressource;
struct Ressource {
    char *filename;
    Token root;
    Ressource *next;
};

/* all tplRessources loaded */
typedef struct RessourceStore RessourceStore;
struct RessourceStore {
    Arena *arena;
    Ressource **res;
};

char* Mstc_ressource_getTypeFromCode(TokenType);

/*
 * Initialize a new store
 */
extern RessourceStore* Mstc_ressource_create();

/*
 * Load file name, store if not done, and return his ressource id.
 */
extern int Mstc_ressource_load(RessourceStore *store, const char *fileName);

/*
 * Return ressource id of filename.
 */
extern int Mstc_ressource_getId(RessourceStore *store, const char *fileName);

/*
 * Get the ressource
 */
extern Ressource*
Mstc_ressource_get(RessourceStore*, const int);

/*
 * Free everything
 */
extern void Mstc_ressource_free(RessourceStore *store);

/*
 * For debuging purpose
 */
extern void Mstc_ressource_printTokenStructure(Ressource *t);


/*******************************************************************************
 * Expander
 ******************************************************************************/
typedef struct ExpandOutput {
    char *out;
    int used;
    int max;
} ExpandOutput;

extern ExpandOutput* Mstc_expand_init(int max);
extern void Mstc_expand_free(ExpandOutput*);
extern void Mstc_expand_run(const Ressource*, const Dict*, ExpandOutput*);

#endif /* _MUSTACHE_H_ */
