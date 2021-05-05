#ifndef __BUBBLE__
#define __BUBBLE__

typedef struct {
    int topo;
    int *balde;
} bucket;

void shuffle (int array[], int n);
int compareSwap (int v[], int i);
void mergesort (int v[], int tam);
void mergesortparallel (int v[], int tam, int cutoff);
void quicksort (int v[], int tam);
void quicksortparallel (int v[], int tam, int cutoff);
void distributeBuckets (bucket b[], int v[], int tam, int num_bucket);

#endif
