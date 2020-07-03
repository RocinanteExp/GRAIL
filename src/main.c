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
    char buffer[200000];

    for(int i = 0; i < 10; i++) { 
        fgets(buffer, 200000, fin); 
        printf("line %s\n", buffer);
    }

    return NULL;
    
}

void test() {

    const int MAX_THREADS = 4;
    fin = fopen("./v500000e1000.gra", "r");
    if(fin == NULL){
        fprintf(stdout, "ERROR opening file\n");
        return;
    }

    pthread_t thread_ids[MAX_THREADS];

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

}

int main(int argc, char **argv) {

    Graph* graph = graph_create("./test/input/grafo20.gra", 5);
    graph_print(graph, true, -1); 
    //Graph* graph = graph_create("./test/input/v500000e1000.gra", 5);
    //graph_print(graph, true, 203); 
    //graph_print(graph, true, 8888); 
    //graph_print(graph, true, 123456); 
    //graph_print(graph, true, 399999); 
    //graph_print(graph, true, 400000); 
    //graph_print(graph, true, 499999); 

    return 0;

}

