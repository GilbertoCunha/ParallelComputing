#ifndef __BUBBLE__
#define __BUBBLE__

typedef struct {
    int topo;
    int *balde;
} bucket;

void shuffle (int array[], int n);
int isOrdered (int v[], int size);
int compareSwap (int v[], int i);
void mergesort (int v[], int tam);
void bubblesort (int v[], int tam);
void quicksort (int v[], int tam);
void distributeBuckets (bucket b[], int v[], int tam, int num_bucket);

#endif
