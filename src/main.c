#include <stdlib.h>
#include <stdio.h>
#include "Label.h"
#include "Graph.h"
#include "Labeling.h"
#include "query.h"
int main(int argc, char **argv){
    
    Graph* graph = graph_create("../test1_100.gra", 2);
    query_init("../test1_50.que", graph); 
    return 0;
}
