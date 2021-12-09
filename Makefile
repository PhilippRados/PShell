# -*- Makefile -*-

CC=gcc-11
CFLAGS=-g #-fsanitize=address

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

.SILENT:
shell: compile_main ## compile the shell
	${CC} ${CFLAGS} src/bin/main.o src/bin/util.o src/bin/fuzzy_finder.o src/bin/tab_complete.o -o src/bin/pshell

start_shell: src/bin/pshell ## start the shell after compilation
	./src/bin/pshell

run_tests: compile_tests compile_main ## Run tests
	${CC} ${CFLAGS} -o ./tests/bin/compiled_tests ./tests/bin/tests.o src/bin/util.o -L/usr/local/Cellar/criterion/2.3.3/lib/ -lcriterion.3.1.0
	./tests/bin/compiled_tests -l
	./tests/bin/compiled_tests

compile_tests:
	${CC} ${CFLAGS} -c ./tests/tests.c -o ./tests/bin/tests.o

compile_main:
	${CC} ${CFLAGS} -c src/util.c -o ./src/bin/util.o
	${CC} ${CFLAGS} -c src/main.c -o ./src/bin/main.o
	${CC} ${CFLAGS} -c src/fuzzy_finder.c -o ./src/bin/fuzzy_finder.o
	${CC} ${CFLAGS} -c src/tab_complete.c -o ./src/bin/tab_complete.o

compile_and_run: shell start_shell
	make shell
	make start_shell

clean: ## cleans up all binary and object files
	rm -f -R *.o ./tests/bin/* ./src/bin/*
