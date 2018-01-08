#include "mustache.h"
#include <stdio.h>

int
main(int argc, char **argv) {

    Dict *d = Mstc_dict_new();
    Mstc_dict_setValue(d, "ok", "okok");
    Mstc_dict_setValue(d, "ok", "okoku");
    Mstc_dict_setValue(d, "o", "ooo");
    Mstc_dict_setValue(d, "name", "nam");
    Mstc_dict_setValue(d, "number", "num");

    Dict *d2;
    d2 = Mstc_dict_addSectionItem(d, "test");
    Mstc_dict_setValue(d2, "name", "jo");
    Mstc_dict_setValue(d2, "genre", "male");
    d2 = Mstc_dict_addSectionItem(d, "test");
    Mstc_dict_setValue(d2, "name", "jojul");
    Mstc_dict_setValue(d2, "genre", "male");
    d2 = Mstc_dict_addSectionItem(d, "test");
    Mstc_dict_setValue(d2, "name", "linda");
    Mstc_dict_setValue(d2, "genre", "female");

    Mstc_dict_setShowSection(d, "test", true);
    Mstc_dict_setShowSection(d, "b", false);

    printf("val for ok is %s\n", Mstc_dict_getValue(d, "ok"));
    printf("val for o is %s\n", Mstc_dict_getValue(d, "o"));
    printf("val for name is %s\n", Mstc_dict_getValue(d, "name"));
    printf("val for number is %s\n", Mstc_dict_getValue(d, "number"));
    printf("should show section %i\n", Mstc_dict_getShowSection(d, "test"));
    printf("should show section %i\n", Mstc_dict_getShowSection(d, "b"));

    Dict **d3;
    int nelem;
    d3 = Mstc_dict_getSection(d, "test1", &nelem);
    printf("have %i elems  %p \n", nelem, d3);

    d3 = Mstc_dict_getSection(d, "test", &nelem);
    printf("have %i elems  %p \n", nelem, d3);

    char *a = Mstc_dict_getValue(d3[2], "name");
    printf("-> %s <-\n",a);
    int i;

    for (i=0; i<nelem; i++) {
        printf("name: %s, genre: %s, other: %s\n",
                Mstc_dict_getValue(d3[i], "name"),
                Mstc_dict_getValue(d3[i], "genre"),
                Mstc_dict_getValue(d3[i], "other"));
    }

    Mstc_dict_free(d);
    return 0;
}
