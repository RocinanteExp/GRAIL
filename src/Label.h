#ifndef LABEL_H_GUARD
#define LABEL_H_GUARD
#include <stdbool.h>

typedef struct label_s
{
    int left,right;
}Label;
//Create a new label
Label initLabel(int l,int r);
//see if l1 is included in l2
bool includeLabel(Label l1,Label l2); 

#endif
