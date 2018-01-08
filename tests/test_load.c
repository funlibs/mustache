#include <stdio.h>

#include "mustache.h"

static char testfile[] = "sample.html.tpl";

int main(int argc, char **argv) {
    RessourceStore *store;
    store = Mstc_ressource_create();

    int i        = Mstc_ressource_load(store, testfile);
    Ressource *r = Mstc_ressource_get(store, i);
    printf("res name is %s\n", r->filename);

    Mstc_ressource_printTokenStructure(r);

    Mstc_ressource_free(store);
    printf("Test tplressource successfull\n");
    return 0;
}
