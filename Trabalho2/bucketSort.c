#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "papi.h"
#include "sorting.h"

// Papi defines
#define NUM_EVENTS 2
#define NUM_RUNS 10 
 
void bucket_sort_parallel (int v[], int tam, int num_buckets, int thread_count) {
    bucket *b = malloc (num_buckets * sizeof (bucket));                                      
    int i, j, k;

    // Inicializar os baldes
    #pragma omp parallel num_threads(thread_count)
    {
        #pragma omp for schedule(dynamic)
        for(i=0; i<num_buckets; i++) {
            b[i].topo = 0; // Inicializar o número de elementos usados no balde
            b[i].balde = malloc (tam * sizeof(int)); // Inicializar o array com o tamanho adequado
        }
    }
    
    // Distribuir os elementos do array pelos baldes
    distributeBucketsParallel(b, v, tam, num_buckets, thread_count);

    // Ordenar os baldes
    #pragma omp parallel num_threads(thread_count)
    {
        #pragma omp for schedule(dynamic)
        for(i=0; i<num_buckets; i++) {
            int tid = omp_get_thread_num();
            if(b[i].topo)
                quicksort (b[i].balde, b[i].topo);
        }
    }

    // Get cumulative sums for bucket sizes
    int sums[num_buckets];
    sums[0] = 0;
    for (i=1; i<num_buckets; ++i) 
        sums[i] = sums[i-1] + b[i-1].topo;

    // Inserir os elementos ordenados dos baldes de volta no vetor
    #pragma omp parallel num_threads(thread_count)
    {
        #pragma omp for schedule(dynamic)
        for(j=0; j<num_buckets; j++) {
            for(k=0; k<b[j].topo; k++) 
                v[sums[j]+k] = b[j].balde[k];
            free (b[j].balde);
        }
    }
    free (b);
}

void bucket_sort (int v[], int tam, int num_buckets) {
    bucket *b = malloc (num_buckets * sizeof (bucket));                                      
    int i, j, k;

    // Inicializar os baldes
    for(i=0; i<num_buckets; i++) {
        b[i].topo = 0; // Inicializar o número de elementos usados no balde
        b[i].balde = malloc (tam * sizeof(int)); // Inicializar o array com o tamanho adequado
    }
         
    // Distribuir os elementos do array pelos baldes
    distributeBuckets (b, v, tam, num_buckets);
         
    // Ordenar os baldes
    for(i=0; i<num_buckets; i++) 
        if(b[i].topo)
            quicksort (b[i].balde, b[i].topo);

    // Inserir os elementos ordenados dos baldes de volta no vetor
    i=0;
    for(j=0; j<num_buckets; j++) {
        for(k=0; k<b[j].topo; k++)
            v[i++] = b[j].balde[k];
        free (b[j].balde);
    }
    free (b);
}


// Add papi metrics
long long metrics[NUM_EVENTS+1];
long long start, stop;
float means[NUM_EVENTS+1];
int Events[NUM_EVENTS] = { PAPI_TOT_INS, PAPI_TOT_CYC };
int EventSet = PAPI_NULL;

int main (int argc, char **argv) {
    // Execution arguments 
    int totalsize = atoi(argv[1]);
    int num_buckets = atoi(argv[2]);
    int thread_count = atoi(argv[3]);
    int is_bucket_parallel = atoi(argv[4]);
    char *sort_func = strdup(argv[5]);
    int cutoff = atoi(argv[6]);

    // Open results csv file
    FILE *f;
    if (access("buckets.csv", F_OK) != 0) {
        f = fopen("buckets.csv", "w");
        fprintf (f, "Total Size,Number of Buckets,Thread Count,Parallel,Sort Function,Cutoff,");
        fprintf (f, "I,CC,Texe,CPI\n"); 
    }
    else f = fopen("buckets.csv", "a");

    // Create and shuffle array 
    int i, j, *v = malloc (totalsize * sizeof (int)); 
    for (i=0; i<totalsize; ++i) v[i] = i;

    // Make array copy
    int *w = malloc (totalsize * sizeof (int));

    // Start papi counters
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);
    PAPI_add_events(EventSet, Events, NUM_EVENTS);
    for (i=0; i<NUM_EVENTS; ++i) means[i] = 0;

    // Create arrays for metrics
    float texe, CPI, CC, I;

    // Perform sorting computation
    for (i=0; i<NUM_RUNS; ++i) {
        printf ("Run %d\n", i);
        // Initialize array copy and shuffle it
        for (j=0; j<totalsize; ++j) w[j] = v[j];
        shuffle (w, totalsize);
        
        // Sort the array
        if (is_bucket_parallel) {
            start = PAPI_get_real_usec();
            PAPI_start(EventSet);
            bucket_sort_parallel (w, totalsize, num_buckets, thread_count);
        }
        else {
            start = PAPI_get_real_usec();
            PAPI_start(EventSet);
            bucket_sort (w, totalsize, num_buckets);
        }
        stop = PAPI_get_real_usec();

        // Stop papi events
        PAPI_stop(EventSet, metrics);
        if (isOrdered(w, totalsize)==1) printf ("SUCCESS\n");

        // Get metrics
        texe = (float) (stop - start);
        I = (float) metrics[0];
        CC = (float) metrics[1];
        CPI = (float) CC / I;

        // Print arguments to csv file
        fprintf (f, "%d,", totalsize);
        fprintf (f, "%d,", num_buckets);
        fprintf (f, "%d,", thread_count);
        fprintf (f, "%d,", is_bucket_parallel);
        fprintf (f, "%s,", sort_func);
        fprintf (f, "%d,", cutoff);

        // Log metrics to csv file
        fprintf (f, "%f,", I);
        fprintf (f, "%f,", CC);
        fprintf (f, "%f,", texe); 
        fprintf (f, "%f\n", CPI);
    }

    fclose (f);

    return 0;
}
