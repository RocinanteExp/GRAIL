CC=gcc
CFLAGS=-pthread -I
main : src/main.c src/label.c src/bitmap.c src/graph.c src/labelling.c src/query.c
	$(CC) src/main.c src/label.c src/bitmap.c src/graph.c src/labelling.c src/query.c -pthread -I "src/include" -Wall -o main
test1 : main.o label.o bitmap.o graph.o labelling.o query.o
	$(CC) -o main main.o label.o bitmap.o graph.o labelling.o query.o -pthread -I "include" 
test:  label.c bitmap.c graph.c labelling.c query.c
	gcc -std=c11  -Wall labeling_and_query_test.c -o test label.c bitmap.c graph.c labelling.c query.c -lcheck -lm -lpthread -lrt -lsubunit -I "../include"
clear: 
	rm -f test
	rm -f main

