#include "mustache.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

static char title_str[] = "title";
KeyHash title;
static char name_str[] = "name";
KeyHash name;
static char genre_str[] = "genre";
KeyHash genre;
static char page_link_str[] = "page_link";
KeyHash page_link;
static char author_str[] = "author";
KeyHash author;
static char url_str[] = "url";
KeyHash url;
static char pets_str[] = "pets";
KeyHash pets;
static char kind_str[] = "kind";
KeyHash kind;
static char footer_str[] = "footer";
KeyHash footer;
static char noescape_str[] = "noescape";
KeyHash noescape;
static char escape_str[] = "escape";
KeyHash escape;
static char childs_str[] = "childs";
KeyHash childs;
static char nest_str[] = "nest";
KeyHash nest;

static char testfile[] = "sample.html.tpl";

void init_keyhash() {
    title = Mstc_dict_genKeyHash(title_str);
    name = Mstc_dict_genKeyHash(name_str);
    genre = Mstc_dict_genKeyHash(genre_str);
    page_link = Mstc_dict_genKeyHash(page_link_str);
    author = Mstc_dict_genKeyHash(author_str);
    url = Mstc_dict_genKeyHash(url_str);
    pets = Mstc_dict_genKeyHash(pets_str);
    kind = Mstc_dict_genKeyHash(kind_str);
    footer = Mstc_dict_genKeyHash(footer_str);
    noescape = Mstc_dict_genKeyHash(noescape_str);
    escape = Mstc_dict_genKeyHash(escape_str);
    childs = Mstc_dict_genKeyHash(childs_str);
    nest = Mstc_dict_genKeyHash(nest_str);
}

Dict*
simulate_request() {
    Dict *dict = Mstc_dict_new();
    Dict *sub, *subsub;
    Mstc_dict_setValue2(dict, &title, "My wonderfull title");
    Mstc_dict_setValue2(dict, &name, "jojo");
    Mstc_dict_setValue2(dict, &genre, "male");
    Mstc_dict_setValue2(dict, &page_link, "<a href=\"https://www.gg.fr\">gg</a>");
    Mstc_dict_setValue2(dict, &author, "jojo");
    Mstc_dict_setValue2(dict, &url, "http://jojo.fr");

    sub = Mstc_dict_addSectionItem2(dict, &pets);
    Mstc_dict_setValue2(sub, &name, "gwen");
    Mstc_dict_setValue2(sub, &kind, "chien");

    sub = Mstc_dict_addSectionItem2(dict, &pets);
    Mstc_dict_setValue2(sub, &name, "chuchen");
    Mstc_dict_setValue2(sub, &kind, "chat");
    subsub = Mstc_dict_addSectionItem2(sub, &nest);
    Mstc_dict_setValue2(subsub, &name, "hello nest");

    Mstc_dict_setValue2(dict, &footer, "copyrithg jojo");
    Mstc_dict_setValue2(dict, &noescape, "<a href=\"#\">jo</a>");
    Mstc_dict_setValue2(dict, &escape, "<a href=\"#\">'jo'</a>");

    return dict;
}

int
main(int argc, char **argv) {
    int count;
    if (argc > 1)
        count = atoi(argv[1]);
    else
        count = 1000;

    init_keyhash();

    RessourceStore *store = Mstc_ressource_create();
    Ressource *ressource = Mstc_ressource_load(store, testfile);

    int i;
    Dict *d;
    char *output;
    for (i=0; i<count; i++) {
        d = simulate_request();
        output = Mstc_expand(ressource, d);
        Mstc_dict_free(d);
        free(output);
    }


    /* one more to print out */
    d = simulate_request();
    output = Mstc_expand(ressource, d);
    printf("%s\n",output);
    free(output);
    Mstc_dict_free(d);
    Mstc_ressource_free(store);

    return 0;
}
