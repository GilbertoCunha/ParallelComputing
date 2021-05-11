#ifndef __BUBBLE__
#define __BUBBLE__

#include "omp.h"

typedef struct {
    int topo;
    int *balde;
    omp_lock_t lock;
} bucket;

void shuffle (int array[], int n);
int compareSwap (int v[], int i);
void mergesort (int v[], int tam);
void mergesortparallel (int v[], int tam, int cutoff);
void quicksort (int v[], int tam);
void quicksortparallel (int v[], int tam, int cutoff);
void distributeBuckets (bucket b[], int v[], int tam, int num_bucket);
void distributeBucketsParallel (bucket b[], int v[], int tam, int num_bucket, int thread_count);

#endif
