#include <stdlib.h>
#include <stdio.h>

#include "mustache_api.h"

static char testfile[] = "sample.html.tpl";

Dict*
simulate_request() {
    Dict *dict = Mstc_dict_new();
    Dict *sub, *subsub;

    Mstc_dict_setValue(dict, "title", "Hey... %s !", "My wonderfull title");
    Mstc_dict_setValue(dict, "name", "%s", "jojo");
    Mstc_dict_setValue(dict, "genre", "%s", "male");
    Mstc_dict_setValue(dict, "page_link", "%s", "<a href=\"https://www.gg.fr\">gg</a>");
    Mstc_dict_setValue(dict, "author", "%s", "jojo");
    Mstc_dict_setValue(dict, "url", "%s", "http://jojo.fr");

    sub = Mstc_dict_addSectionItem(dict, "pets");
    Mstc_dict_setValue(sub, "name", "%s", "gwen");
    Mstc_dict_setValue(sub, "kind", "%s", "chien");

    sub = Mstc_dict_addSectionItem(dict, "pets");
    Mstc_dict_setValue(sub, "name", "%s", "chuchen");
    Mstc_dict_setValue(sub, "kind", "%s", "chat");

    subsub = Mstc_dict_addSectionItem(sub, "nest");
    Mstc_dict_setValue(subsub, "name", "%s", "hello nest");

    Mstc_dict_setValue(dict, "footer", "%s", "copyrithg jojo");
    Mstc_dict_setValue(dict, "noescape", "%s", "<a href=\"#\">jo</a>");
    Mstc_dict_setValue(dict, "escape", "%s", "<a href=\"#\">'jo'</a>");

    return dict;
}

int
main(int argc, char **argv) {
    int count;
    if (argc > 1)
        count = atoi(argv[1]);
    else
        count = 1000;

    TemplateStore *store = Mstc_template_create();
    Template *template = Mstc_template_get(store, testfile);

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
