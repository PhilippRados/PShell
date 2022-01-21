# -*- Makefile -*-

CC = gcc-11
CFLAGS = -g -fsanitize=address -fsanitize=leak
OBJECTS = src/bin/main.o src/bin/util.o src/bin/tab_complete.o src/bin/fuzzy_finder.o
TEST_OBJECTS = tests/bin/*.o

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

src/bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

tests/bin/%.o: tests/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.SILENT:
shell: ${OBJECTS} ## compile the shell
	${CC} ${CFLAGS} ${OBJECTS} -o src/bin/pshell -ldl -lm

start_shell: src/bin/pshell ## start the shell after compilation
	./src/bin/pshell

run_tests: ${TEST_OBJECTS} ## Run tests
	${CC} ${CFLAGS} -o ./tests/bin/compiled_tests ./tests/bin/test_fuzzyfind.o \
		./tests/bin/test_tab_completion.o ./tests/bin/test_util.o ./tests/bin/test_main.o src/bin/util.o src/bin/tab_complete.o src/bin/fuzzy_finder.o  \
		-L/usr/local/Cellar/criterion/2.3.3/lib/ -lcriterion.3.1.0
	./tests/bin/compiled_tests -l
	./tests/bin/compiled_tests

compile_and_run: shell
	make shell && ./src/bin/pshell

clean: ## cleans up all binary and object files
	rm -f -R *.o ./tests/bin/* ./src/bin/*
