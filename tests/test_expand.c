#include "mustache.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

static char testfile[] = "sample.html.tpl";

int
main(int argc, char **argv) {
    Dict *dict = Mstc_dict_new();
    Dict *sub, *subsub;
    Mstc_dict_setValue(dict, "title", "My wonderfull title");
    Mstc_dict_setValue(dict, "name", "jojo");
    Mstc_dict_setValue(dict, "genre", "male");
    Mstc_dict_setValue(dict, "page_link", "<a href=\"https://www.gg.fr\">gg</a>");
    Mstc_dict_setValue(dict, "author", "jojo");
    Mstc_dict_setValue(dict, "url", "http://jojo.fr");

    Mstc_dict_setShowSection(dict, "pets", true);

    sub = Mstc_dict_addSectionItem(dict, "pets");
    Mstc_dict_setValue(sub, "name", "gwen");
    Mstc_dict_setValue(sub, "kind", "chien");

    sub = Mstc_dict_addSectionItem(dict, "pets");
    Mstc_dict_setValue(sub, "name", "chuchen");
    Mstc_dict_setValue(sub, "kind", "chat");

    Mstc_dict_setShowSection(sub, "nest", true);
    subsub = Mstc_dict_addSectionItem(sub, "nest");
    Mstc_dict_setValue(subsub, "name", "nest thing");


    Mstc_dict_setValue(dict, "footer", "copyrithg jojo");
    Mstc_dict_setValue(dict, "noescape", "<a href=\"#\">jo</a>");
    Mstc_dict_setValue(dict, "escape", "<a href=\"#\">'jo'</a>");

    RessourceStore *store = Mstc_ressource_create();
    int index = Mstc_ressource_load(store, testfile);
    Ressource *res = Mstc_ressource_get(store, index);

    int i, j;
    Dict **d = Mstc_dict_getSection(dict, "pets", &i);
    printf("have %i count\n", i);
    for (j=0; j<i; j++) {
        Dict *e = d[j];
        printf("have %s\n", Mstc_dict_getValue(e, "name"));
        printf("have %s\n", Mstc_dict_getValue(e, "kind"));
    }

    //Mstc_ressource_printTokenStructure(res);

    ExpandOutput *exp = Mstc_expand_init(5000);
    Mstc_expand_run(res, dict, exp);

    printf("\n%s\n", exp->out);

    Mstc_expand_free(exp);
    Mstc_ressource_free(store);
    Mstc_dict_free(dict);

    return 0;
}
