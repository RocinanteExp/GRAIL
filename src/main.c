#include <stdlib.h>
#include <stdio.h>
#include "Label.h"
#include "Graph.h"
int main(int argc, char* argv[])
{
   /* // test label
    Label l1,l2,l3,l4;
    l1= initLabel(1,5);
    l2=initLabel(2,3);
    l3= initLabel(4,6);
    l4= initLabel(6,9);
    printf("l1: %d %d\n", l1.left,l1.right);
    printf("l2: %d %d\n", l2.left,l2.right);
    printf("l3: %d %d\n", l3.left,l3.right);
    printf("l4: %d %d\n", l4.left,l4.right);
    printf("Res for l2 l1 %d expected 1 \n",includeLabel(l2,l1));
    printf("Res for l1 l2 %d expected 0 \n",includeLabel(l1,l2));
    printf("Res for l3 l1 %d expected 0 \n",includeLabel(l3,l1));
    printf("Res for l4 l1 %d expected 0 \n",includeLabel(l4,l1));*/
    // test Node
    Node* x=create_node(2,0);
    int i=0;
    char* str="0: #";
    set_childrens(x,str);
    for(;i<x->num_childrens;i++)
        printf("%d ",x->childrens[i]);
    printf("\n");
    destroy_node(x);
    return 0;
}
