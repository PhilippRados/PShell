# -*- Makefile -*-

CC = gcc-11
CFLAGS = -g -fsanitize=address -fsanitize=leak
TEST_COVERAGE = -fprofile-arcs -ftest-coverage
LIB_COVERAGE = -fprofile-arcs -lgcov --coverage
OBJECTS = src/bin/main.o src/bin/util.o src/bin/tab_complete.o src/bin/fuzzy_finder.o
OBJECTS_NO_MAIN = src/bin/util.o src/bin/tab_complete.o src/bin/fuzzy_finder.o
TEST_OBJECTS = tests/bin/test_main.o tests/bin/test_util.o tests/bin/test_fuzzy_finder.o tests/bin/test_tab_complete.o
TEST_SOURCES = tests/test_main.c tests/test_util.c tests/test_fuzzy_finder.c tests/test_tab_complete.c
test_target = $(basename $(notdir $(TEST_SOURCES)))

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

src/bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

tests/bin/%.o: tests/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I /usr/local/Cellar/criterion/2.3.3/include/ -L/usr/local/Cellar/criterion/2.3.3/lib  -lcriterion.3.1.0 

.SILENT:
shell: ${OBJECTS} ## Compile the shell
	${CC} ${CFLAGS} ${OBJECTS} -o src/bin/pshell -ldl -lm

start_shell: src/bin/pshell ## Start the shell after compilation
	./src/bin/pshell

run_tests: $(TEST_OBJECTS) $(OBJECTS_NO_MAIN) ## Run all tests (needs criterion)
	${CC} ${CFLAGS} -o ./tests/bin/compiled_tests ${TEST_OBJECTS} ${OBJECTS_NO_MAIN} \
		-I /usr/local/Cellar/criterion/2.3.3/include/ -L/usr/local/Cellar/criterion/2.3.3/lib  -lcriterion.3.1.0
	./tests/bin/compiled_tests -l
	./tests/bin/compiled_tests

$(test_target): $(TEST_OBJECTS) $(OBJECTS) ## Run individual tests (needs criterion)
	if [ $@ = "test_util" -o $@ = "test_main" ]; then\
		${CC} ${CFLAGS} -o ./tests/bin/compiled_tests tests/bin/$@.o src/bin/util.o -L/usr/local/Cellar/criterion/2.3.3/lib/ -I/usr/local/Cellar/criterion/2.3.3/lib/include/criterion -lcriterion.3.1.0;\
	else\
		${CC} ${CFLAGS} -o ./tests/bin/compiled_tests tests/bin/$@.o src/bin/util.o src/bin/$(subst test_,'',$@).o -L/usr/local/Cellar/criterion/2.3.3/lib/ -I/usr/local/Cellar/criterion/2.3.3/lib/include/criterion -lcriterion.3.1.0;\
  fi
	./tests/bin/compiled_tests -l
	./tests/bin/compiled_tests

compile_and_run: shell
	make shell && ./src/bin/pshell

clean: ## Cleans up all binary and object files
	rm -f -R *.o ./tests/bin/* ./src/bin/*
