#include <stdlib.h>
#include <stdio.h>
#include "Graph.h"


Node* create_node(int d, int n, unsigned int id)
{
    Node* node= malloc(sizeof(Node));
    node->num_childrens=n;
    node->id=id;
    node->intervals= malloc(sizeof(Label)*d);
    node->interval_bitmap=bitmap_create(d);
    node->childrens=(unsigned int*)calloc(n,sizeof(unsigned int));
}

void destroy_node(Node* node)
{
    free(node->intervals);
    bitmap_destroy(node->interval_bitmap);
    free(node->childrens);
    free(node);
}