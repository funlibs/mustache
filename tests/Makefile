TESTBINS = test_dict test_load test_expand test_expand2

.PHONY: tests clean check profile
tests: $(TESTBINS)

test_dict: test_dict.c ../mustache.h
	$(CC) $(CFLAGS) -static test_dict.c $(LDFLAGS) -o test_dict
test_expand2: test_expand2.c ../mustache.h
	$(CC) $(CFLAGS) -static test_expand2.c $(LDFLAGS) -o test_expand2
test_expand: test_expand.c ../mustache.h
	$(CC) $(CFLAGS) -static test_expand.c $(LDFLAGS) -o test_expand
test_load: test_load.c ../mustache.h
	$(CC) $(CFLAGS) -static test_load.c $(LDFLAGS) -o test_load


clean:
	$(RM) $(TESTBINS)

.PHONY: check
check: $(TESTBINS)
	./test_dict > /dev/null || echo "test_dict fail"
	./test_load > /dev/null || echo "test_load fail"
	./test_expand2 > /dev/null || echo "test_expand fail"

.PHONY: profile
profile: tests
	sudo perf stat -d ./test_expand2 300000 > /dev/null
	@echo "Press enter to continue"; read A
	sudo perf record -e task-clock,cycles,instructions,cache-references,cache-misses,branches,branch-misses ./test_expand2 300000 > /dev/null
	sudo perf report
	@#valgrind ./test_expand2.bin 10 > /dev/null;

