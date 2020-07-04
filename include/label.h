#ifndef LABEL_H_GUARD
#define LABEL_H_GUARD
#include <stdbool.h>
#include <stdint.h>

typedef struct label_s
{
    uint64_t left,right;
}Label;
//Create a new label
Label label_init(uint64_t l,uint64_t r);
//see if l1 is included in l2
bool label_include(Label l1,Label l2); 

#endif
