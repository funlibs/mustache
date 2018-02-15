/* MIT License Copyright 2018  Sebastien Serre */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "mustache_api.h"

#define EXP_OUT_MAX 5000
typedef struct ExpandOutput {
    char *out;
    unsigned int used;
    unsigned int max;
} ExpandOutput;

/* token handlers */
static void handle_string(
    const Token*, const Dict*, ExpandOutput*);
static void handle_key_noescape(
    const Token*, const Dict*, ExpandOutput*);
static void handle_key(
    const Token*, const Dict*, ExpandOutput*);
static void handle_inv_section(
    const Token*, const Dict*, ExpandOutput*);
static void handle_section(
    const Token*, const Dict*, ExpandOutput*);
static void handle_bool_section(
    const Token*, const Dict*, ExpandOutput*);
static void handle_root_section(
    const Token*, const Dict*, ExpandOutput*);
static void (*handlers[7])(const Token*, const Dict*, ExpandOutput*) = {
    handle_string,
    handle_key,
    handle_key_noescape,
    handle_section,
    handle_inv_section,
    handle_bool_section,
    handle_root_section};


/* exape html chars with minimal branches */
static char doublequot[] = "&#34;";
static char amp[]        = "&#38;";
static char singlequot[] = "&#39;";
static char lt[]         = "&#60;";
static char gt[]         = "&#62;";
static char *ascii_table[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0 ... 24 */
    0,0,0,0,0,0,0,0,0,doublequot,0,0,0,amp,singlequot,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,lt,0,gt,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0}; /* ..255 */
static int
do_escape(
    char *b,
    char *str,
    size_t len)
{
    char *c;
    int n = len;

    for (;len;len--) {
        if ((c = ascii_table[(unsigned char) *str])) {
            (*b++) = c[0];
            (*b++) = c[1];
            (*b++) = c[2];
            (*b++) = c[3];
            (*b++) = c[4];
            n += 4; /* four more chars */
            str++;
        } else {
            (*b++) = *str++;
        }
    }

    *b = '\0';
    return n;
}


/* api */
char*
Mstc_expand(
    const Template *template,
    const Dict *dict)
{
    ExpandOutput exp;
    exp.out = malloc(EXP_OUT_MAX);
    exp.max = EXP_OUT_MAX;
    exp.used = 0;
    handle_root_section(&template->root, dict, &exp);
    exp.out[exp.used] = '\0';
    return exp.out;
}

/* handlers */
static void
handle_string(
    const Token *t,
    const Dict *dict,
    ExpandOutput *exp)
{
    /* handle TokenType.STRING_TOKEN */

    StaticString *stvalue;
    if ((stvalue = (StaticString*) t->value) == NULL) return;

    if ((exp->max - exp->used) < (stvalue->len + 1)) {
        exp->out = realloc(exp->out, exp->max * 3);
        exp->max *= 3;
    }

    memcpy(&exp->out[exp->used], stvalue->str, stvalue->len);

    exp->used += stvalue->len;
}


static void
handle_key_noescape(
    const Token *t,
    const Dict *dict,
    ExpandOutput *exp)
{
    /* handle TokenType.KEY_TOKEN_NO_ESCAPE */
    size_t len;
    char *value;

    if ((value = Mstc_dict_getValue2(dict, (KeyHash*) t->value)) == NULL)
        return;

    len = strlen(value);
    if ((exp->max - exp->used) < len + 1) {
        exp->out = realloc(exp->out, exp->max * 3);
        exp->max *= 3;
    }

    memcpy(&exp->out[exp->used], value, len);

    exp->used += len;
}


static void
handle_key(
    const Token *t,
    const Dict *dict,
    ExpandOutput *exp)
{
    /* handle TokenType.KEY_TOKEN */
    size_t len;
    char *value;

    if ((value = Mstc_dict_getValue2(dict, (KeyHash*) t->value)) == NULL)
        return;

    len = strlen(value);
    if ((exp->max - exp->used) < (len * 5 + 1)) {
        exp->out = realloc(exp->out, exp->max * 3);
        exp->max *= 3;
    }

    len = do_escape(&exp->out[exp->used], value, len);

    exp->used += len;
}


static void
handle_inv_section(
    const Token *t,
    const Dict *dict,
    ExpandOutput *exp)
{
    /* handle TokenType.INV_SECTION_TOKEN */
    int i;

    if (Mstc_dict_getShowSection2(dict, (KeyHash*) t->value) == true)
        return;

    for (i=0; i<t->nchilds; i++)
        handlers[(&t->childs[i])->type](&t->childs[i], dict, exp);
}


static void
handle_root_section(
    const Token *t,
    const Dict *dict,
    ExpandOutput *exp)
{
    /* handle TokenType.ROOT_SECTION_TOKEN */

    int i;
    for (i=0; i<t->nchilds; i++)
        handlers[(&t->childs[i])->type](&t->childs[i], dict, exp);
}


static void
handle_bool_section(
    const Token *t,
    const Dict *dict,
    ExpandOutput *exp)
{
    /* handle TokenType.BOOL_SECTION_TOKEN */
    int i;

    if (Mstc_dict_getShowSection2(dict, (KeyHash*) t->value) == false)
        return;

    for (i=0; i<t->nchilds; i++)
        handlers[(&t->childs[i])->type](&t->childs[i], dict, exp);
}


static void
handle_section(
    const Token *t,
    const Dict *dict,
    ExpandOutput *exp)
{
    /* handle TokenType.SECTION_TOKEN */

    int n;
    Dict **subs = Mstc_dict_getSection2(dict, (KeyHash*) t->value, &n);

    int i, k;
    for (i=0; i<n; i++)
        for (k=0; k<t->nchilds; k++)
            handlers[(&t->childs[k])->type](&t->childs[k], subs[i], exp);
}
