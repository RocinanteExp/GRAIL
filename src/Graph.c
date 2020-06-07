#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph.h"
#include "bitmap.h"
#define DEBUG 0 

Node* node_create(int d, unsigned int id)
{

    Node* node = malloc(sizeof(Node));
    if(node == NULL)
        return NULL;
    node->num_children=0;
    node->id = id;
    node->intervals = malloc(sizeof(Label)*d);
    node->interval_bitmap = bitmap_create(d);
    node->children = NULL;
    
    return node;

}

void node_destroy(Node* node)
{

    free(node->intervals);
    bitmap_destroy(node->interval_bitmap);
    free(node->children);
    free(node);

}

void node_set_children(Node* node, char* str)
{
    uint32_t n=0;
    uint32_t i=0;
    uint32_t str_length = strlen(str);
    char* s = malloc(str_length + 1);
    char* context;
    char* tok;
    errno_t error = strcpy_s(s, str_length + 1, str);
    if(error != 0){
        fprintf(stderr, "ERROR: strcpy set_children\n");
        return; 
    }
#if DEBUG
    printf("lunghezza stringa parametro 'str' %llu\n", str_length);
    printf("[%s] [%s]\n", s, str);
    printf("FINE PRIMA STAMPA\n");
#endif
    tok=strtok_s(s, ": #\n", &context);
    while((tok = strtok_s(NULL, ": #\n", &context)) != NULL)
    {
        n++;
    }

    node->children = (uint32_t*) calloc(n, sizeof(uint32_t));

    error = strcpy_s(s, str_length + 1, str);
    if(error != 0){
        fprintf(stderr, "ERROR: strcpy set_children\n");
        return; 
    }

    tok = strtok_s(s, ": #", &context);
    while((tok = strtok_s(NULL, ": #\n", &context)) != NULL && i < n)
    {
        node->children[i] = (unsigned int) atoi(tok);
        i++;
    }

    node->num_children = n;

#if DEBUG
    node_print(node);
    printf("\n");
    printf("--------------------------------\n");
#endif

}

Graph* graph_create(char *filepath, int num_intervals){
    
    const uint16_t BUFF_SIZE = 1024; 

    FILE *fin;
    errno_t error = fopen_s(&fin, filepath, "r");
    if(error != 0){
        fprintf(stdout, "ERROR opening file %s at graph_create\n", filepath);
        return NULL;
    }

    Graph* p_graph = malloc(sizeof(Graph));
    if(p_graph == NULL){
        fprintf(stdout, "ERROR on malloc of 'graph' at graph_create\n");
        return NULL;
    }

    uint32_t num_nodes;
    char curr_line[BUFF_SIZE];
    fgets(curr_line, BUFF_SIZE, fin);
    sscanf_s(curr_line, "%d", &num_nodes);
    
    p_graph->nodes = malloc(num_nodes * sizeof(Node*));
    if(p_graph->nodes == NULL){
        free(p_graph);
        fprintf(stdout, "ERROR on malloc of 'graph->nodes' at graph_create\n");
        return NULL;
    }

    Bitmap* bitmap = bitmap_create(num_nodes);
    // create the nodes of the graph
    for(int i = 0; i < num_nodes; i++){

        fgets(curr_line, BUFF_SIZE, fin);
        Node* curr_node = node_create(num_intervals, i);
        if(curr_node == NULL){
            fprintf(stderr, "ERROR: failed node_create at iteration %d of graph_create\n", i);
            for(int j = 0; j < i; j++){
                free(p_graph->nodes[j]);
            }
            free(p_graph->nodes);
            free(p_graph);
            return NULL;
        }
            
        node_set_children(curr_node, curr_line);
        p_graph->nodes[i] = curr_node;
    
        //TODO implement it as a function
        for(int i = 0; i < curr_node->num_children; i++)
            bitmap_set_bit(bitmap, curr_node->children[i]);

    }

    p_graph->num_intervals = num_intervals;
    p_graph->num_nodes = num_nodes;
    
    //finding the root nodes of the graph
    //TODO do it a better way. Add checks.
    uint32_t next = 0;
    uint16_t MAGIC_NUMBER = 128;
    p_graph->root_nodes = malloc(MAGIC_NUMBER * sizeof(uint32_t));
    for(int i = 0; i < p_graph->num_nodes; i++){
        if(!bitmap_test_bit(bitmap, i)){
            p_graph->root_nodes[next++] = i;
        }
        if(next >= MAGIC_NUMBER){
            MAGIC_NUMBER *= 2;
            realloc(p_graph->root_nodes, MAGIC_NUMBER);
        }
    }
    p_graph->num_root_nodes = next;

    fclose(fin);
    return p_graph;

}

void graph_destroy(Graph *graph){

    if(graph == NULL)
        return;

    for(int i = 0; i < graph->num_nodes; i++){
        node_destroy(graph->nodes[0]);
    }

    free(graph->root_nodes);
    free(graph->nodes);
    free(graph);

}

//TODO implement possibility to print a single node instead of all the graph
void graph_print(Graph *graph, bool verbose, uint32_t index_node){

    fprintf(stdout, "PRINTING GRAPH\n");
    int i = 0;
    while(i < graph->num_nodes){
        node_print(graph->nodes[i++], false);
        fprintf(stdout, "\n");
    }

    fprintf(stdout, "ROOT NODES\n");
    for(int i = 0; i < graph->num_root_nodes; i++){
        fprintf(stdout, "%d ", graph->root_nodes[i]);
    }

    fprintf(stdout, "\n");
    fprintf(stdout, "FINISHED PRINTING GRAPH\n");

}

//TODO implement a "verbose" version of printing a node
void node_print(Node *node, bool verbose){

    printf("%d: ", node->id);
    for(int i = 0; i < node->num_children; i++){
        printf("%d ", node->children[i]);
    }
    printf("#");

}
