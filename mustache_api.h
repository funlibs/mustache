/* MIT License Copyright 2018  Sebastien Serre */

#ifndef _MUSTACHE_API_H_
#define _MUSTACHE_API_H_
#include "mustache.h"


/*******************************************************************************
 * templates
 ******************************************************************************/
/* Initialize a new store */
extern TemplateStore* Mstc_template_create();

/* Free everything */
extern void Mstc_template_free(TemplateStore *store);

/* Load file name, store it if not done, and return it.*/
extern Template*
Mstc_template_get(TemplateStore *store, const char *filename);

/* Expand template with dict values */
extern char* Mstc_expand(const Template*, const Dict*);


/*******************************************************************************
 * dictionary
 ******************************************************************************/
/* Will create a "main" dictionary. This is the one we will want to
 * free with Mstc_dict_free(). */
extern Dict* Mstc_dict_new();

 /* Set text value */
extern void Mstc_dict_setValue(Dict *dict, char *key, const char *format, ...);

/* inconditionaly hide or show section */
extern void Mstc_dict_setShowSection(Dict *dict, char *key, const bool show);

/* do not free these dicts! */
extern Dict* Mstc_dict_addSectionItem(Dict *dict, char *key);

/* Free the main dictionnary.*/
extern void Mstc_dict_free(Dict* dict);


#endif /* _MUSTACHE_API_H_ */
