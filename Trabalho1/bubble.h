#ifndef __BUBBLE__
#define __BUBBLE__

/*
typedef struct {
    int topo;
    int balde[tam_bucket];
} bucket;
*/

typedef struct {
    int topo;
    int *balde;
} bucket;

void shuffle (int array[], int n);
int isOrdered (int v[], int size);
int compareSwap (int v[], int i);

// Different Versions of bubble sort
void bubble (int v[], int tam);
void bubble1 (int v[], int tam);
void bubble2 (int v[], int tam);
void bubble3 (int * __restrict__ v, int tam);
void mergesort (int v[], int tam);

// Bucket array element Distribution
void distributeBuckets (bucket b[], int v[], int tam, int num_bucket);
void distributeBuckets1 (bucket b[], int v[], int tam, int num_bucket);


void bucket_sort1 (int * __restrict__ v, int tam, int num_bucket);

#endif