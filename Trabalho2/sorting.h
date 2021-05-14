#ifndef __BUBBLE__
#define __BUBBLE__

#include <omp.h>

typedef struct {
    int topo;
    int *balde;
    omp_lock_t lock;
} bucket;

void shuffle (int array[], int n);
int isOrdered (int v[], int size);
int compareSwap (int v[], int i);
void quicksort (int v[], int tam);
void distributeBuckets (bucket b[], int v[], int tam, int num_bucket);
void distributeBucketsParallel (bucket b[], int v[], int tam, int num_bucket, int thread_count);
#endif
