# -*- Makefile -*-

CC = gcc-11
CFLAGS = -g -fsanitize=address -fsanitize=leak
OBJECTS = src/bin/main.o src/bin/util.o src/bin/tab_complete.o src/bin/fuzzy_finder.o src/bin/readline.o
SOURCES = src/main.c src/util.c src/tab_complete.c src/fuzzy_finder.c src/readline.c
TEST_OBJECTS = tests/unit_tests/bin/test_main.o tests/unit_tests/bin/test_util.o tests/unit_tests/bin/test_fuzzy_finder.o tests/unit_tests/bin/test_tab_complete.o tests/unit_tests/bin/test_readline.o
TEST_SOURCES = tests/unit_tests/test_main.c tests/unit_tests/test_util.c tests/unit_tests/test_fuzzy_finder.c tests/unit_tests/test_tab_complete.c tests/unit_tests/test_readline.c
test_target = $(basename $(notdir $(TEST_SOURCES)))
CURRENT_DIR = $(shell pwd)

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

src/bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

tests/bin/%.o: tests/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I /usr/local/Cellar/criterion/2.3.3/include/ -L/usr/local/Cellar/criterion/2.3.3/lib  -lcriterion.3.1.0 

.SILENT:
shell: ${OBJECTS} ## Compile the shell
	${CC} ${CFLAGS} ${OBJECTS} -o src/bin/pshell -ldl -lm

docker_shell:
	gcc src/main.c src/util.c src/fuzzy_finder.c src/tab_complete.c src/readline.c -o src/bin/pshell -ldl -lm

start_shell: src/bin/pshell ## Start the shell after compilation
	./src/bin/pshell

run_tests: $(TEST_SOURCES) $(SOURCES) ## Run all tests (needs criterion)
	${CC} ${CFLAGS} -o ./tests/unit_tests/bin/compiled_tests -D TEST ${TEST_SOURCES} ${SOURCES} \
		-I /usr/local/Cellar/criterion/2.3.3/include/ -L/usr/local/Cellar/criterion/2.3.3/lib -lcriterion.3.1.0
	./tests/unit_tests/bin/compiled_tests -l
	./tests/unit_tests/bin/compiled_tests

$(test_target): $(TEST_SOURCES) $(SOURCES) ## Run individual tests (needs criterion)
	if [ $@ = "test_util" -o $@ = "test_main" -o $@ = "test_readline" ]; then\
		${CC} ${CFLAGS} -o ./tests/unit_tests/bin/compiled_$(subst test_,'',$@)_tests -D TEST tests/unit_tests/$@.c $(SOURCES) -L/usr/local/Cellar/criterion/2.3.3/lib/ -I/usr/local/Cellar/criterion/2.3.3/include/ -lcriterion.3.1.0;\
	else\
		${CC} ${CFLAGS} -o ./tests/unit_tests/bin/compiled_$(subst test_,'',$@)_tests -D TEST tests/unit_tests/$@.c src/util.c src/$(subst test_,'',$@).c -L/usr/local/Cellar/criterion/2.3.3/lib/ -I/usr/local/Cellar/criterion/2.3.3/include/ -lcriterion.3.1.0;\
  fi
	./tests/unit_tests/bin/compiled_$(subst test_,'',$@)_tests -l
	./tests/unit_tests/bin/compiled_$(subst test_,'',$@)_tests

integration_tests: #./tests/integration_tests/test_pshell.rb ## (Only works with Ruby and ttytest)
	make clean
	docker run -ti --rm -v $(CURRENT_DIR):/pshell testing_container \
		"make docker_shell && cat ./tests/integration_tests/autocomplete_tests.txt >> /root/.psh_history && ruby ./tests/integration_tests/test_pshell.rb"
	make clean

compile_and_run: shell
	make shell && ./src/bin/pshell

clean: ## Cleans up all binary and object files
	rm -f -R *.o ./tests/unit_tests/bin/* ./src/bin/*
