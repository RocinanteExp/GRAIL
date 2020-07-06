#include <stdlib.h>
#include <stdio.h>
#include "label.h"
#include "graph.h"
#include "labelling.h"
#include "query.h"
#include <time.h>
#include <pthread.h>
static FILE *fin;

void malloc_performance(){
    Node** array = malloc(500000*sizeof(Node*));
    clock_t start = clock();
    for(int i = 0; i < 500000; i++) { 
        array[i] = malloc(sizeof(Node));
    }

    clock_t end = clock();
    printf("DIFFERENCE single %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    for(int i = 0; i < 500000; i++) { 
        free(array[i]);
    }

    start = clock();
    for(int i = 0; i < 500000/10; i++) { 
        malloc(10*sizeof(Node));
    }
    end = clock();
    printf("DIFFERENCE x 10 %f\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    for(int i = 0; i < 500000/20; i++) { 
        malloc(20*sizeof(Node));
    }
    end = clock();
    printf("DIFFERENCE x 20 %f\n", (double)(end - start) / CLOCKS_PER_SEC);
}

void* start_thread(void* arg) {
    char buffer[30];
    static uint32_t iter = 0;
    while(fgets(buffer, 30, fin)) { 
        if(iter < 30) {
            iter++;
            printf("line %s\n", buffer);
        }
    }

    printf("line %s\n", buffer);

    return NULL;
};

void test() {

    const int MAX_THREADS = 1;
    fin = fopen("test/input/v500000e1000.que", "r");
    if(fin == NULL){
        fprintf(stdout, "FAILED opening file at test\n");
        return;
    }

    pthread_t thread_ids[MAX_THREADS];

    clock_t start = clock();
    for(int i = 0; i < MAX_THREADS; i++) {
        int err = pthread_create(&thread_ids[i], NULL, start_thread, NULL); 
        if(err != 0) {
            fprintf(stderr, "ERROR: pthread_create %d", i);
            exit(2);
        };
    }

    for(int i = 0; i < MAX_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    clock_t end = clock();
    printf("DIFFERENCE %f\n", (double)(end - start) / CLOCKS_PER_SEC);

}

int main(int argc, char **argv) {

#if 0
    Graph* graph = graph_create("test/input/v500000e1000.gra", 2);
    graph_randomize_labelling_sequential(graph, 2);
    query_init("test/input/v500000e1000.que", graph);
#else
    Graph* graph = graph_create("test/input/grafo20.gra", 2);
    graph_randomize_labelling_sequential(graph, 2);
    graph_print(graph, true, -1); 
    query_init("test/input/grafo20_25.que", graph);
#endif

#if 0
    clock_t start = clock();
    graph_print_to_file("test/input/out.gra", graph); 
    clock_t end = clock();
    fprintf(stdout, "FINISCHED WRITING FILE\n");
    printf("It took %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
#endif

    return 0;

}

