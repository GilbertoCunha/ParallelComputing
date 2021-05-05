#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "papi.h"
#include "bubble.h"

// Papi defines
#define NUM_EVENTS 2
#define NUM_RUNS 200 
 
void bucket_sort_parallel (int v[], int tam, int num_buckets, int thread_count, char *sort_func, int cutoff, FILE *out) {
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
    #pragma omp parallel num_threads(thread_count)
    #pragma omp for schedule(dynamic)
    for(i=0; i<num_buckets; i++) {
        //#pragma omp task firstprivate(i)
        int tid = omp_get_thread_num();
        if(b[i].topo) {
            // mergesort (b[i].balde, b[i].topo);
            // mergesortparallel (b[i].balde, b[i].topo, cutoff);
            if (strcmp (sort_func, "quicksort") == 0)
                quicksort (b[i].balde, b[i].topo);
            else if (strcmp (sort_func, "quicksortparallel") == 0) 
                quicksortparallel (b[i].balde, b[i].topo, cutoff);
        }
        // fprintf (out, "\tThread %d finished sorting bucket %d\n", tid, i);
    }

    // Inserir os elementos ordenados dos baldes de volta no vetor
    i=0;
    for(j=0; j<num_buckets; j++) {
        for(k=0; k<b[j].topo; k++)
            v[i++] = b[j].balde[k];
        free (b[j].balde);
    }
    free (b);
}

void bucket_sort (int v[], int tam, int num_buckets, char *sort_func, int cutoff, FILE *out) {
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
    for(i=0; i<num_buckets; i++) {
        //#pragma omp task firstprivate(i)
        if(b[i].topo) {
            // mergesort (b[i].balde, b[i].topo);
            // mergesortparallel (b[i].balde, b[i].topo, cutoff);
            if (strcmp (sort_func, "quicksort") == 0)
                quicksort (b[i].balde, b[i].topo);
            else if (strcmp (sort_func, "quicksortparallel") == 0) 
                quicksortparallel (b[i].balde, b[i].topo, cutoff);
        } 
        // fprintf (out, "\tFinished sorting bucket %d\n", i);
    }

    // Inserir os elementos ordenados dos baldes de volta no vetor
    i=0;
    for(j=0; j<num_buckets; j++) {
        for(k=0; k<b[j].topo; k++)
            v[i++] = b[j].balde[k];
        free (b[j].balde);
    }
    free (b);
}

float mean (float v[], int n) {
    float r = 0;
    for (int i=0; i<n; ++i) r += v[i];
    r /= n;
    return r;
}

float std (float v[], int n) {
    float r = 0;
    float vmean = mean (v, n);
    for (int i=0; i<n; ++i) r += pow (v[i] - vmean, 2);
    r = sqrt(r / n);
    return r; 
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
    FILE *f, *out = fopen("out.log", "w");
    if (access("results.csv", F_OK) != 0) {
        f = fopen("results.csv", "w");
        fprintf (f, "Total Size,Number of Buckets,Thread Count,Parallel,Sort Function,Cutoff,");
        fprintf (f, "I mean,I std,CC mean,CC std,Texe mean,Texe std,CPI mean,CPI std\n"); 
    }
    else f = fopen("results.csv", "a");

    // Create and shuffle array 
    int *v = malloc (totalsize * sizeof (int)); 
    for (int i=0; i<totalsize; ++i) v[i] = i;

    // Make array copy
    int *w = malloc (totalsize * sizeof (int));

    // Start papi counters
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);
    PAPI_add_events(EventSet, Events, NUM_EVENTS);
    for (int i=0; i<NUM_EVENTS; ++i) means[i] = 0;

    // Create arrays for metrics
    float texe[NUM_RUNS], CPIS[NUM_RUNS], CC[NUM_RUNS], I[NUM_RUNS];

    // Perform sorting computation
    for (int i=0; i<NUM_RUNS; ++i) {
        // Initialize array copy and shuffle it
        fprintf (out, "Iteration %d: \n", i);
        for (int j=0; j<totalsize; ++j) w[j] = v[j];
        shuffle (w, totalsize);
        
        // Initialize papi events
        
        // Sort the array
        if (is_bucket_parallel) {
            start = PAPI_get_real_usec();
            PAPI_start(EventSet);
            bucket_sort_parallel (w, totalsize, num_buckets, thread_count, sort_func, cutoff, out);
        }
        else {
            start = PAPI_get_real_usec();
            PAPI_start(EventSet);
            bucket_sort (w, totalsize, num_buckets, sort_func, cutoff, out);
        }
        stop = PAPI_get_real_usec();

        // Stop papi events
        PAPI_stop(EventSet, metrics);

        // Check if array is sorted
        if (isOrdered(w, totalsize)) fprintf (out, "\tSUCCESS\n\n");
        else fprintf (out, "\tFAILURE\n\n");

        // Get metrics
        texe[i] = (float) (stop - start);
        I[i] = (float) metrics[0];
        CC[i] = (float) metrics[1];
        CPIS[i] = (float) CC[i] / I[i];
    }

    // Print mean metrics
    fprintf (out, "ARGUMENTS: \n");
    fprintf (out, "Totalsize: %d\n", totalsize);
    fprintf (out, "Number of Buckets: %d\n", num_buckets);
    fprintf (out, "Thread Count: %d\n", thread_count);
    fprintf (out, "Is bucket parallel: %d\n", is_bucket_parallel);
    fprintf (out, "Sorting Function: %s\n", sort_func);
    fprintf (out, "Cutoff: %d\n", cutoff);
    fprintf (out, "\nMETRICS: \n");
    fprintf (f, "%d,", totalsize);
    fprintf (f, "%d,", num_buckets);
    fprintf (f, "%d,", thread_count);
    fprintf (f, "%d,", is_bucket_parallel);
    fprintf (f, "%s,", sort_func);
    fprintf (f, "%d,", cutoff);
    
    fprintf (out, "CC mean: %.2f | CC std: %.2f", mean(CC, NUM_RUNS), std(CPIS, NUM_RUNS));
    fprintf (out, "Execution time mean: %.2f us | Execution time std: %.2f\n", mean(texe, NUM_RUNS), std(CPIS, NUM_RUNS));
    fprintf (out, "CPI mean: %.2f | CPI std: %.2f\n", mean(CPIS, NUM_RUNS), std(CPIS, NUM_RUNS));
    fprintf (out, "#I mean: %.2f | #I std: %.2f\n", mean(I, NUM_RUNS), std(I, NUM_RUNS));

    fprintf (f, "%f,%f,", mean(I, NUM_RUNS), std(I, NUM_RUNS));
    fprintf (f, "%f,%f,", mean(CC, NUM_RUNS), std(CPIS, NUM_RUNS));
    fprintf (f, "%f,%f,", mean(texe, NUM_RUNS), std(CPIS, NUM_RUNS));
    fprintf (f, "%f,%f\n", mean(CPIS, NUM_RUNS), std(CPIS, NUM_RUNS));


    fclose (f);

    return 0;
}
