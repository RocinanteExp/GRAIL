CC=gcc
CFLAGS=-pthread -I
test1 : main.o label.o bitmap.o graph.o labelling.o query.o
	$(CC) -o main main.o label.o bitmap.o graph.o labelling.o query.o -pthread -I "include" 
main : main.c label.c bitmap.c graph.c labelling.c query.c
	$(CC) main.c label.c bitmap.c graph.c labelling.c query.c -pthread -I "include" -o main
test:  label.c bitmap.c graph.c labelling.c query.c
	gcc -std=c11  -Wall labeling_and_query_test.c -o test label.c bitmap.c graph.c labelling.c query.c -lcheck -lm -lpthread -lrt -lsubunit
clear: 
	rm -f test
	rm -f main

