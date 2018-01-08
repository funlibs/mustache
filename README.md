MUSTACHE ...
========
... a mustache like templating engine. Mostly Stick to mustache spec and add the
{{?mysection}} tag (if section is not empty), use full file path in {{>}}
tags, no lambdas.

Mustache was created for my self experiment on processor caches, branches prediction
optimisation techniques, and profiling tools.

MustacheRessourceStore has the lifetime of the application, MustacheDict of a request.
See *test_expand2.c* for a complete usage example.

It is not production code: no documentation, no tests, behaviour with
incorrectly written templates is undefined. It is just fast.

[https://github.com/mustache/mustache](https://github.com/mustache/mustache)

[https://github.com/OlafvdSpek/ctemplate](https://github.com/OlafvdSpek/ctemplate)

# TODO
* API mustache.h
* utf8 tests
