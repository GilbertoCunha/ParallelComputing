#ifndef __BUBBLE__
#define __BUBBLE__

typedef struct {
    int topo;
    int *balde;
} bucket;

void shuffle (int array[], int n);
int isOrdered (int v[], int size);
double mean (double v[], int n);
void quicksort (int v[], int tam);
void distributeBuckets (bucket b[], int v[], int tam, int num_bucket);
#endif
