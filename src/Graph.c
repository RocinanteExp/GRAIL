#include <stdlib.h>
#include <stdio.h>
#include "Graph.h"
#include <string.h>

Node* create_node(int d, unsigned int id)
{
    Node* node= malloc(sizeof(Node));
    node->num_childrens=0;
    node->id=id;
    node->intervals= malloc(sizeof(Label)*d);
    node->interval_bitmap=bitmap_create(d);
    node->childrens=NULL;
}

void destroy_node(Node* node)
{
    free(node->intervals);
    bitmap_destroy(node->interval_bitmap);
    free(node->childrens);
    free(node);
}

void set_childrens(Node* node,char* str)
{
    unsigned int n=0;
    unsigned int x;
    unsigned int i=0;
    char* s;
    char* tok;
    strcpy(s,str);
    tok=strtok(s,": #");
   while(tok= strtok(NULL,": #"))
   {
       n++;
   }
    node->childrens=(unsigned int*)calloc(n,sizeof(unsigned int));
    strcpy(s,str);
    tok=strtok(s,": #");
   while(tok=strtok(NULL,": #") && i<n)
   {
       node->childrens[i]=(unsigned int)atoi(tok);
       i++;
   }
    node->num_childrens=n;
}