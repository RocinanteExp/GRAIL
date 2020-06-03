#ifndef LABEL_H_GUARD
#define LABEL_H_GUARD

typedef struct label_s
{
    int left,right;
}Label;

Label initLabel(int l,int r);
int includeLabel(Label l1,Label l2); //see if l1 is included in l2
#endif