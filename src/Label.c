#include <stdlib.h>
#include <stdio.h>
#include "Label.h"
#define TRUE 1
#define FALSE 0

Label initLabel(int l,int r)
{
    Label x;
    x.left=l;
    x.right=r;
    return x;
}

int includeLabel(Label l1,Label l2)
{
    if(l1.left<l2.left || l1.right>l2.right)
        return FALSE;
    return TRUE;
}