MUSTACHE
========

Mustache templating engine implementation. Mostly stick to Mustache spec but:
* add the {{?mysection}} tag (if section is not empty)
* use the full file path in include tags {{>}},
* no lambdas. 

This was created for my self-experiments on processor caches/branches
optimisation techniques, and profiling tools. Most of the optimization work
is related to the expand step (*expand.c*). It is fast. It does also contains
utilities present in the original Ctemplate library (caching templates and dictionary).

See *Makefile* for build instructions.

See *test_expand2.c* for a complete usage example.

[https://github.com/mustache/mustache](https://github.com/mustache/mustache)

[https://github.com/OlafvdSpek/ctemplate](https://github.com/OlafvdSpek/ctemplate)

### TODO
* utf8 tests

### License
MIT License Copyright 2018  Sebastien Serre
