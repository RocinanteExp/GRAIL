# -*- Makefile -*-

CC=gcc
IDIR=./include
SDIR=./src
CFLAGS=-pthread -Wall -g -O0

main: $(SDIR)/*.c $(IDIR)/*.h
	$(CC) $(SDIR)/*.c $(CFLAGS) -I $(IDIR) -o bin/main
test_query: src/label.c src/bitmap.c src/graph.c src/query.c test/query_test.c
	gcc test/query_test.c src/label.c src/bitmap.c src/graph.c src/query.c -lpthread -lcheck -lm -lrt -lsubunit -I $(IDIR) -o bin/test
test_label: src/label.c src/bitmap.c src/graph.c src/query.c test/labeling_and_query_test.c
	gcc test/label_test.c src/label.c src/bitmap.c src/graph.c src/query.c -g -lcheck -lm -lrt -lsubunit -pthread -I $(IDIR) -o bin/test
test_node: src/label.c src/bitmap.c src/graph.c src/query.c test/labeling_and_query_test.c
	gcc test/node_test.c src/label.c src/bitmap.c src/graph.c src/query.c -lcheck -lm -lrt -lsubunit -pthread -I $(IDIR) -o bin/test
clean: 
	rm -f bin/main
cleanWin: 
	rm bin\main.exe

