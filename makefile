# -*- Makefile -*-

CC=gcc
IDIR=./include
SDIR=./src
CFLAGS=-pthread -Wall -g -O3
TEST_SOURCE_FILES=src/label.c src/bitmap.c src/graph.c src/query.c src/time_tracker.c
main: $(SDIR)/*.c $(IDIR)/*.h
	$(CC) $(SDIR)/*.c $(CFLAGS) -I $(IDIR) -o bin/main
test_query: $(TEST_SOURCE_FILES) test/tests/query_test.c
	gcc test/tests/query_test.c $(TEST_SOURCE_FILES) -g -lpthread -lcheck -lm -lrt -lsubunit -I $(IDIR) -o bin/test
test_label: $(TEST_SOURCE_FILES) test/tests/label_test.c
	gcc test/tests/label_test.c $(TEST_SOURCE_FILES) -g -lcheck -lm -lrt -lsubunit -pthread -I $(IDIR) -o bin/test
test_node: $(TEST_SOURCE_FILES) test/tests/node_test.c
	gcc test/tests/node_test.c $(TEST_SOURCE_FILES) -g -lcheck -lm -lrt -lsubunit -pthread -I $(IDIR) -o bin/test
test_graph: $(TEST_SOURCE_FILES) test/tests/graph_test.c
	gcc test/tests/graph_test.c $(TEST_SOURCE_FILES) -g -lcheck -lm -lrt -lsubunit -pthread -I $(IDIR) -o bin/test
test_bitmap: $(TEST_SOURCE_FILES) test/tests/bitmap_test.c
	gcc test/tests/bitmap_test.c $(TEST_SOURCE_FILES) -g -lcheck -lm -lrt -lsubunit -pthread -I $(IDIR) -o bin/test
clean: 
	rm -f bin/*
cleanWin: 
	rm bin\*

