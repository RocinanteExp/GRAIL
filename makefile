CC=gcc
CFLAGS=-pthread -I
main : src/main.c src/label.c src/bitmap.c src/graph.c src/labelling.c src/query.c
	$(CC) src/main.c src/label.c src/bitmap.c src/graph.c src/labelling.c src/query.c -pthread -I "src/include" -Wall -g -o src/main
test:  label.c bitmap.c graph.c labelling.c query.c
	gcc -std=c11  -Wall labeling_and_query_test.c -o test label.c bitmap.c graph.c labelling.c query.c -lcheck -lm -lpthread -lrt -lsubunit -I "../include"
clear: 
	rm -f test
	rm -f main

