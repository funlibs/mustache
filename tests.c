#include <stdlib.h>
#include <stdio.h>

#include "mustache_api.h"

Dict*
simulate_request() {
    Dict *dict = Mstc_dict_new();
    Dict *sub, *subsub;

    Mstc_dict_setValue(dict, "title", "My wonderfull title");
    Mstc_dict_setValue(dict, "name", "jojo");
    Mstc_dict_setValue(dict, "genre", "male");
    Mstc_dict_setValue(dict, "page_link", "<a href=\"https://www.gg.fr\">gg</a>");
    Mstc_dict_setValue(dict, "author", "jojo");
    Mstc_dict_setValue(dict, "url", "http://jojo.fr");

    sub = Mstc_dict_addSectionItem(dict, "pets");
    Mstc_dict_setValue(sub, "name", "gwen");
    Mstc_dict_setValue(sub, "kind", "chien");

    sub = Mstc_dict_addSectionItem(dict, "pets");
    Mstc_dict_setValue(sub, "name", "chuchen");
    Mstc_dict_setValue(sub, "kind", "chat");

    subsub = Mstc_dict_addSectionItem(sub, "nest");
    Mstc_dict_setValue(subsub, "name", "hello nest");

    Mstc_dict_setValue(dict, "footer", "copyrithg jojo");
    Mstc_dict_setValue(dict, "noescape", "<a href=\"#\">jo</a>");
    Mstc_dict_setValue(dict, "escape", "<a href=\"#\">'jo'</a>");

    return dict;
}

int
main(int argc, char **argv) {
    int count;
    if (argc > 2)
        count = atoi(argv[2]);
    else
        count = 1000;


    TemplateStore *store = Mstc_template_create();
    Template *template = Mstc_template_get(store, argv[1]);

    Dict *dict;
    char *output;
    int i;
    for (i=0; i<count; i++) {
        dict = simulate_request();
        output = Mstc_expand(template, dict);
        Mstc_dict_free(dict);
        free(output);
    }

    /* one more to print out */
    dict = simulate_request();
    output = Mstc_expand(template, dict);
    Mstc_dict_free(dict);

    printf("%s\n",output);
    free(output);

    Mstc_template_free(store);
    return EXIT_SUCCESS;
}
