/* MIT License Copyright 2018  Sebastien Serre */

#ifndef _MUSTACHE_H_
#define _MUSTACHE_H_
#include "mustache_int.h"

/*******************************************************************************
 * simple templates
 ******************************************************************************/
extern Template* Mstc_template_open(const char *filename);
extern void Mstc_template_close(Template *tpl);


/*******************************************************************************
 * templates store
 ******************************************************************************/
/* Initialize a new store */
extern TemplateStore* Mstc_template_create();

/* Free everything */
extern void Mstc_template_free(TemplateStore *store);

/* Get template "filename", load it if not done. */
extern Template* Mstc_template_get(TemplateStore *store, const char *filename);


/*******************************************************************************
 * dictionary
 ******************************************************************************/
/* Will create a "main" dictionary. This is the one we will want to
 * free with Mstc_dict_free(). */
extern Dict* Mstc_dict_new();

/* Set simple text */
extern void Mstc_dict_setValue(Dict *dict, char *key, const char *value);

/* Set text with printf like arguments. Return the len of the formated string */
extern int Mstc_dict_setFValue(Dict *dict, char *key, const char *format, ...);

/* inconditionaly hide or show section */
extern void Mstc_dict_setShowSection(Dict *dict, char *key, const bool show);

/* do not free these dicts! */
extern Dict* Mstc_dict_addSectionItem(Dict *dict, char *key);

/* Free the main dictionnary.*/
extern void Mstc_dict_free(Dict* dict);


/*******************************************************************************
 * expand template with dict
 ******************************************************************************/
/* Expand template with dict values */
extern char* Mstc_expand(const Template*, const Dict*);

#endif /* _MUSTACHE_H_ */
