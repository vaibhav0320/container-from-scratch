CC=gcc
CFLAGS=-u

main: main.cpp
	gcc main.cpp -o main

run: main
	./main

