#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "label.h"

Label label_init(uint64_t l,uint64_t r)
{
    Label x;
    x.left=l;
    x.right=r;
    return x;
}

bool label_include(Label l1,Label l2)
{
    if(l1.left<l2.left || l1.right>l2.right)
        return false;
    return true;
}
