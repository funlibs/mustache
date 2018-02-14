/* MIT License Copyright 2018  Sebastien Serre */

#ifndef _MUSTACHE_API_H_
#define _MUSTACHE_API_H_
#include "mustache.h"


/*******************************************************************************
 * templates
 ******************************************************************************/
/* Initialize a new store */
extern TemplateStore* Mstc_template_create();

/* Load file name, store it if not done, and return it.*/
extern Template*
Mstc_template_load(TemplateStore *store, const char *fileName);

/* Get a template, same as Mstc_template_load*/
extern Template*
Mstc_template_get(TemplateStore *store, const char *filename);

/* Free everything */
extern void Mstc_template_free(TemplateStore *store);


/*******************************************************************************
 * dictionary
 ******************************************************************************/
/* Will create a "main" dictionary. This is the one we will want to
 * free with Mstc_dict_free(). */
Dict* Mstc_dict_new();

 /* Set text value */
void Mstc_dict_setValue(Dict *dict, char *key, const char *value);

/* set formated key value like printf */
void Mstc_dict_setFValue(Dict *dict, char *key, const char *format, ...);

/* setShowSection(false), will hide a sections (of the same name) that
 * contains some values and which would have been evaluated to true without
 * this explicit false.
 * showsection(true), will inconditionaly show the section, even if there is no
 * content in the section.*/
void Mstc_dict_setShowSection(Dict *dict, char *key, const bool show);

/* do not free these dicts! */
Dict* Mstc_dict_addSectionItem(Dict *dict, char *key);

/* Free the main dictionnary.*/
void Mstc_dict_free(Dict* dict);


/*******************************************************************************
 * expand
 ******************************************************************************/
extern char* Mstc_expand(const Template*, const Dict*);


#endif /* _MUSTACHE_API_H_ */
