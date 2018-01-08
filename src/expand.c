#include "mustache.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


static void
handle_string_token(const Token*, const Dict*, ExpandOutput*);
static void
handle_key_token_noescape(const Token*, const Dict*, ExpandOutput*);
static void
handle_key_token(const Token*, const Dict*, ExpandOutput*);
static void
handle_inv_section_token(const Token*, const Dict*, ExpandOutput*);
static void
handle_section_token(const Token*, const Dict*, ExpandOutput*);
static void
handle_bool_section_token(const Token*, const Dict*, ExpandOutput*);
static void
handle_root_section_token(const Token*, const Dict*, ExpandOutput*);
static void (*handlers[7])(const Token*, const Dict*, ExpandOutput*) = {
    handle_string_token,
    handle_key_token,
    handle_key_token_noescape,
    handle_section_token,
    handle_inv_section_token,
    handle_bool_section_token,
    handle_root_section_token
};
static inline int do_escape(char *b, char *str, size_t len);

void
Mstc_expand_run(const Ressource *ressource, const Dict *dict,
        ExpandOutput* exp)
{
    exp->used = 0; // reset
    handle_root_section_token(&ressource->root, dict, exp);
    exp->out[exp->used] = '\0';
}

ExpandOutput*
Mstc_expand_init(int max) {

    ExpandOutput *exp = malloc(sizeof(ExpandOutput));
    exp->out = malloc(max);
    exp->max = max;
    exp->used = 0;
    return exp;
}

void
Mstc_expand_free(ExpandOutput* exp) {
    free(exp->out);
    free(exp);
}

static void
handle_string_token(const Token *t, const Dict *dict, ExpandOutput *exp)
{
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
handle_key_token_noescape(
        const Token *t, const Dict *dict, ExpandOutput *exp)
{
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
handle_key_token(
        const Token *t, const Dict *dict, ExpandOutput *exp)
{
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
handle_inv_section_token(
        const Token *t, const Dict *dict, ExpandOutput *exp)
{
    int i;
    static void (*f)(const Token*, const Dict*, ExpandOutput*);
    if (Mstc_dict_getShowSection2(dict, (KeyHash*) t->value) == true)
        return;

    for (i=0; i<t->nchilds; i++) {
        f = handlers[(&t->childs[i])->type];
        f(&t->childs[i], dict, exp);
    }

}

static void
handle_root_section_token(
        const Token *t, const Dict *dict, ExpandOutput *exp)
{
    int i;
    static void (*f)(const Token*, const Dict*, ExpandOutput*);
    for (i=0; i<t->nchilds; i++) {
        f = handlers[(&t->childs[i])->type];
        f(&t->childs[i], dict, exp);
    }
}


static void
handle_bool_section_token(
        const Token *t, const Dict *dict, ExpandOutput *exp)
{
    int i;
    static void (*f)(const Token*, const Dict*, ExpandOutput*);
    if (Mstc_dict_getShowSection2(dict, (KeyHash*) t->value) == false)
        return;
    for (i=0; i<t->nchilds; i++) {
        f = handlers[(&t->childs[i])->type];
        f(&t->childs[i], dict, exp);
    }

}

static void
handle_section_token(
        const Token *t, const Dict *dict, ExpandOutput *exp)
{
    int i, j, n;
    static void (*f)(const Token*, const Dict*, ExpandOutput*);
    Dict **subs = Mstc_dict_getSection2(dict, (KeyHash*) t->value, &n);

    for (i=0; i<n; i++) {
        for (j=0; j<t->nchilds; j++) {
            f = handlers[(&t->childs[j])->type];
            f(&t->childs[j], subs[i], exp);
        }
    }

}


static char doublequot[] = "&#34;";
static char amp[]        = "&#38;";
static char singlequot[] = "&#39;";
static char lt[]         = "&#60;";
static char gt[]         = "&#62;";
static char *ascii_table[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0 -> 24 */
    0,0,0,0,0,0,0,0,0,doublequot,0,0,0,amp,singlequot,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,lt,0,gt,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0};
static inline int
do_escape(char *b, char *str, size_t len)
{
    char *c;
    int n = len;

    for (; len > 0; len--) {
        if ((c = ascii_table[(unsigned char) *str])) {
            (*b++) = c[0];
            (*b++) = c[1];
            (*b++) = c[2];
            (*b++) = c[3];
            (*b++) = c[4];
            n += 4;
            str++;
        } else {
            (*b++) = *str++;
        }
    }

    *b = '\0';
    return n;
}

