main : main.c Label.c bitmap.c Graph.c Labeling.c query.c
	gcc -o main main.c Label.c bitmap.c Graph.c Labeling.c query.c -lm
test:  Label.c bitmap.c Graph.c Labeling.c query.c
	gcc -std=c11  -Wall labeling_and_query_test.c -o test Label.c bitmap.c Graph.c Labeling.c query.c -lcheck -lm -lpthread -lrt -lsubunit
clear: 
	rm -f test
	rm -f main