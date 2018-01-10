MUSTACHE ...
========
... a mustache like templating engine. Stick to Mustache spec and add the
{{?mysection}} tag (if section is not empty), use full file path in {{>}}
tags, no lambdas. Reimplement caching techniques used in the original
Ctemplate libary.

Mustache was created for my self experiment on processor caches and branches
optimisation techniques, and profiling tools. Most of the optimization work
is related to the expand step (*expand.c*). It is fast.

See *test_expand2.c* for a complete usage example.

[https://github.com/mustache/mustache](https://github.com/mustache/mustache)

[https://github.com/OlafvdSpek/ctemplate](https://github.com/OlafvdSpek/ctemplate)

# TODO
* utf8 tests

# License
MIT License Copyright 2018  Sebastien Serre
