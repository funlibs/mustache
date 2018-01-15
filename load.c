/* MIT License Copyright 2018  Sebastien Serre */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mustache.h"

#define RES_ARENA_BASE_SIZE 16777216 /* around 15 mb */
#define RES_DICT_INIT_SIZE 50
#define ROOT_SECTION_TOKEN_INITIAL_SIZE 20


/* forward declaration */
static Token do_load(const char*, Arena*);
static void do_trim(char*);
static void insert_token(Token*, const Token, Arena*);
static void set_token_staticstr(Token*, const char*, const int, Arena*);
static char* get_filename(char*, const int, Arena*);
static int do_tokenize(Token*, char**, Arena*);
static void print_token(Token*);
static void insert_res(Ressource**, const char*, Arena*);


RessourceStore*
Mstc_ressource_create()
{
    Arena *arena = Arena_new(RES_ARENA_BASE_SIZE);
    RessourceStore *store = Arena_malloc(arena, sizeof(RessourceStore));
    store->arena = arena;
    store->res = Arena_calloc(arena,
            RES_DICT_INIT_SIZE * sizeof(Ressource*));
    return store;
}


int
Mstc_ressource_load(
    RessourceStore *store, 
    const char *filename)
{
    int hash = djb2_hash(filename) % RES_DICT_INIT_SIZE;
    insert_res(&store->res[hash], filename, store->arena);
    return hash;
}


int
Mstc_ressource_getId(
    RessourceStore *store, 
    const char *filename)
{
    return Mstc_ressource_load(store, filename);
}


Ressource*
Mstc_ressource_get(
    RessourceStore *store, 
    const int id)
{
    return store->res[id];
}


void
Mstc_ressource_free(RessourceStore *store)
{
    Arena_free(store->arena);
}


void Mstc_ressource_printTokenStructure(Ressource *t)
{
    print_token(&t->root);
}


const char type_root[]        = "root_section";
const char type_key[]         = "key";
const char type_keynoescape[] = "key_no_escape";
const char type_section[]     = "section";
const char type_boolsection[] = "bool_section";
const char type_invsection[]  = "inv_section";
const char type_string[]      = "string";
char*
Mstc_ressource_getTypeFromCode(TokenType t)
{
    switch(t) {
        case ROOT_SECTION_TOKEN:
            return (char*) type_root;
        case KEY_TOKEN:
            return (char*) type_key;
        case KEY_TOKEN_NO_ESCAPE:
            return (char*) type_keynoescape;
        case SECTION_TOKEN:
            return (char*) type_section;
        case INV_SECTION_TOKEN:
            return (char*) type_invsection;
        case BOOL_SECTION_TOKEN:
            return (char*) type_boolsection;
        case STRING_TOKEN:
            return (char*) type_string;
        default:
            return (char*) NULL;
    }
}


/* static */
static void
do_trim(char *str)
{
    int trim_left = 0, trim_right = 0;
    int len = strlen(str);
    int i;
    for (i=0; i<len - 1; i++)
        if (str[i] == ' ')
            trim_left++;
        else
            break;

    for (i=len - 1; i>1; i--)
        if (str[i] == ' ')
            trim_right++;
        else
            break;

    if (trim_left > 0) {
        for (i=trim_left; i<len; i++)
            str[i - trim_left] = str[i];
        str[len - trim_left] = '\0';
    }

    if (trim_right > 0)
        str[strlen(str) - trim_right] = '\0';
}


static void
insert_token(
    Token *root, 
    const Token child, 
    Arena *arena)
{
    if (!child.value) return; /* a comment */

    if (root->size == 0) {
        root->childs = Arena_malloc(arena,
                sizeof(Token) * ROOT_SECTION_TOKEN_INITIAL_SIZE);
        root->size = ROOT_SECTION_TOKEN_INITIAL_SIZE;
    } else if (root->size == root->nchilds) {
        int old_size = sizeof(Token) * root->size;
        root->childs = Arena_realloc(arena, root->childs, old_size, old_size * 3);
        root->size *= 3;
    }
    root->childs[root->nchilds] = child;
    root->nchilds++;
}


static void
set_token_keystr(
    Token *t, 
    const char *str, 
    const int len, 
    Arena *arena)
{
    KeyHash *h = Arena_malloc(arena, sizeof(KeyHash));
    char *str2 = Arena_malloc(arena, len + 1);
    strncpy(str2, str, len);
    str2[len] = '\0';
    do_trim(str2);
    *h = Mstc_dict_genKeyHash(str2);
    t->value = (void*) h;
}


/* may overlaps */
static inline void
strmove(char *dest, char *src)
{
    while((*src) != '\0') {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = '\0';

}


static void
tinyfy(char *str)
{
    char *skip_start = NULL;

    while(*(str) != '\0' ) {
        if (*str == ' ' || *str == '\n' || *str == '\t')  {
            if (!skip_start) {
                skip_start = str;
                *skip_start = ' '; /* replace newline and tabs with spaces */
            }
        } else {
            if (skip_start) {
                skip_start++;

                if (skip_start < str)
                    strmove(skip_start, str);
                str = skip_start;
                skip_start = NULL;
            }
        }
        str++;
    }

    if (skip_start) {
        *(++skip_start) = '\0';
    }
}


static void
set_token_staticstr(
    Token *t, 
    const char *str, 
    const int len, 
    Arena *arena)
{
    /* build a temporary array to store mignified string in arena */
    char *buff = malloc(len + 1);
    strncpy(buff, str, len);
    buff[len] = '\0';
    tinyfy(buff);
    int bufflen = strlen(buff);

    StaticString *statstr = Arena_malloc(arena, sizeof(StaticString));
    statstr->str = Arena_malloc(arena, bufflen + 1);

    strcpy(statstr->str, buff);
    statstr->len = bufflen;

    t->value = (void*) statstr;
}


static char*
get_filename(
    char *str, 
    const int len, 
    Arena *arena)
{
    char *r = Arena_malloc(arena, len + 1);
    strncpy(r, str, len);
    r[len] = '\0';
    return r;
}


static const char token_start[] = "{{";
static const char token_end[]   = "}}";
static int
do_tokenize(
    Token *t, 
    char **current, 
    Arena *arena)
{
    int s;
    if (strlen(*current) == 0) /* end of processing */
        return 0;

    t->childs = NULL;
    t->nchilds = 0;
    t->size = 0;

    char *start = strstr(*current, token_start);

    if (start == NULL) {
        /* no more start tags in string. This token is the end of string*/
        t->type = STRING_TOKEN;

        set_token_staticstr(t, *current, strlen(*current), arena);
        *current += strlen(*current);
        return 1;

    } else if (start != *current) {
        /* got some string before start tag, this token is a string */
        t->type = STRING_TOKEN;

        set_token_staticstr(t, *current, start - *current, arena);
        *current = start;
        return 1;

    } else {
        /* here, start == *current, wee have a valid tag */

        /* eliminate our begin of tag symbol {{ */
        start +=2;

        /* get "end" and eliminate }} for next occurence */
        char *end = strstr(start, token_end);
        *current = end + 2;

        /* eliminate carriage return */
        /*if (end[2] == '\n')
            (*current)++;*/

        Token child;
        /* handle our tag */
        switch(start[0]) {
            case '/':       /* end of section tag return 0 */
                return 0;
            case '!':       /* comment do nothing */
                t->value = NULL;
                return 1;
            case '&':       /* do not escape */
                start++;
                t->type = KEY_TOKEN_NO_ESCAPE;
                set_token_keystr(t, start, end - start, arena);
                return 1;
            case '{':       /* basic triple, do not escape HTML */
                start++;
                t->type = KEY_TOKEN_NO_ESCAPE;
                set_token_keystr(t, start, end - start, arena);
                *current += 1; /* eliminate 3rd closing bracket */
                return 1;
            case '#':       /* start of section tag */
                start++;
                t->type = SECTION_TOKEN;
                set_token_keystr(t, start, end - start, arena);
                while (do_tokenize(&child, current, arena)) {
                    insert_token(t, child, arena);
                }
                return 1;
            case '?':       /* start of boolean section tag */
                start++;
                t->type = BOOL_SECTION_TOKEN;
                set_token_keystr(t, start, end - start, arena);
                while ((s = do_tokenize(&child, current, arena))) {
                    insert_token(t, child, arena);
                }
                return 1;
            case '^':       /* start of inverted section tag */
                start++;
                t->type = INV_SECTION_TOKEN;
                set_token_keystr(t, start, end - start, arena);
                while (do_tokenize(&child, current, arena)) {
                    insert_token(t, child, arena);
                }
                return 1;
            case '>':     /* load external file */
                start++;
                *t = do_load(get_filename(start, end - start, arena), arena);
                return 1;
            default:      /* basic tag */
                t->type = KEY_TOKEN;
                set_token_keystr(t, start, end - start, arena);
                return 1;
        }
    }
}


static int pos = 0;
static void
print_token(Token *t)
{
    int i,j = 0;

    char *out;
    if (t->type == STRING_TOKEN) {
        out = ((StaticString*) t->value)->str;
        j = ((StaticString*) t->value)->len;
    } else if (t->type == ROOT_SECTION_TOKEN) {
        out = (char*) t->value;
    } else {
        out = ((KeyHash*) t->value)->str;
    }
    printf("%*sTOKEN %s value: %s (%i)\n", pos, "",
            Mstc_ressource_getTypeFromCode(t->type), out, j);
    if (t->nchilds > 0) {
        pos += 4;
        for (i=0; i<t->nchilds; i++) {
            print_token(&t->childs[i]);
        }
        pos -= 4;
        printf("%*sEND TOKEN SECTION\n", pos, "");
    }
}


static Token
do_load(
    const char *filename, 
    Arena *arena)
{
    /* read file into memory */
    FILE *fp;
    fp = fopen(filename, "r");
    assert(fp != NULL);

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    char *str = malloc(fsize + 1);
    if (fread(str, 1, fsize, fp) != fsize)
        abort();
    fclose(fp);
    str[fsize] = '\0';

    /* create root tokens */
    Token root;
    root.type = ROOT_SECTION_TOKEN;
    root.value = Arena_malloc(arena, strlen(filename) + 1);
    strcpy(root.value, filename);
    root.childs = Arena_malloc(arena,
            sizeof(Token) * ROOT_SECTION_TOKEN_INITIAL_SIZE);
    root.nchilds = 0;
    root.size = ROOT_SECTION_TOKEN_INITIAL_SIZE;

    Token child;
    char *current = str;
    while (do_tokenize(&child, &current, arena)) {
        insert_token(&root, child, arena);
    }

    /* free everything */
    free(str);
    return root;
}


static void
insert_res(
    Ressource **res, 
    const char *filename, 
    Arena *arena)
{
    if (*res == NULL) {
        *res = Arena_malloc(arena, sizeof(Ressource));
        (*res)->filename = Arena_malloc(arena, sizeof(char) * strlen(filename));
        strcpy((*res)->filename, filename);
        (*res)->next = NULL;
        (*res)->root = do_load(filename, arena);
        return;
    } else if (strcmp((*res)->filename, filename) == 0) { /* allready loaded */
        return;
    } else {    /* find next */
        insert_res(&(*res)->next, filename, arena);
    }
}

