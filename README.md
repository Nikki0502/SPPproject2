Build Docker Container:

docker build -t myallocator .

Run docker in console:

docker run -it --rm -v "${PWD}:/app" myallocator /bin/bash

Automaticly runs tests with make file bc of dockerfile:

docker run -it --rm -v "${PWD}:/app" myallocator

oder einfach make test

entferne -ddbug flag wenn nicht debugging

debugging mit gdb :

gdb --args env LD_PRELOAD=./liballoc.so ./test_prog   

testing mem leaks mit valgrind :

valgrind --leak-check=full --show-leak-kinds=all ./test_prog
