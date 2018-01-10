TESTBINS = test_dict test_load test_expand test_expand2

.PHONY: tests clean check profile
tests: $(TESTBINS)

%: %.c
	$(CC) $(CFLAGS) -static $< $(LDFLAGS) -o $@

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
