#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "papi.h"
#include "bubble.h"

// Papi defines
#define NUM_EVENTS 2
#define NUM_RUNS 20 
 
// Tamanho total do array
#define totalsize 10000000

// BucketSort defines
#define thread_count 12 // Número de threads
#define num_bucket 10 // Número de baldes
#define cutoff 1000 // Cutoff do sorting paralelo
#define tam_bucket (int) totalsize/num_bucket // Tamanho total de cada balde

void bucket_sort (int v[], int tam) {       
                                  
    bucket *b = malloc (num_bucket * sizeof (bucket));                                      
    int i, j, k;

    // Inicializar os baldes
    for(i=0; i<num_bucket; i++) {
        b[i].topo = 0; // Inicializar o número de elementos usados no balde
        b[i].balde = malloc (tam * sizeof(int)); // Inicializar o array com o tamanho adequado
    }
         
    // Distribuir os elementos do array pelos baldes
    distributeBuckets (b, v, tam, num_bucket);
         
    // Ordenar os baldes
    #pragma omp parallel num_threads(thread_count)
    #pragma omp for schedule(dynamic)
    for(i=0; i<num_bucket; i++) {
        //#pragma omp task firstprivate(i)
        int tid = omp_get_thread_num();
        if(b[i].topo) {
            // mergesort (b[i].balde, b[i].topo);
            // mergesortparallel (b[i].balde, b[i].topo, cutoff);
            quicksort (b[i].balde, b[i].topo);
            // quicksortparallel (b[i].balde, b[i].topo, cutoff);
        } 
        printf ("\tThread %d finished sorting bucket %d\n", tid, i);
    }

    // Inserir os elementos ordenados dos baldes de volta no vetor
    i=0;
    for(j=0; j<num_bucket; j++) {
        for(k=0; k<b[j].topo; k++)
            v[i++] = b[j].balde[k];
        free (b[j].balde);
    }
    free (b);
}

// Add papi metrics
long long metrics[NUM_EVENTS];
long long start, stop;
float texe = 0;
float means[NUM_EVENTS];
int Events[NUM_EVENTS] = { PAPI_TOT_INS, PAPI_TOT_CYC };
int EventSet = PAPI_NULL;

int main () {
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

    // Perform sorting computation
    for (int i=0; i<NUM_RUNS; ++i) {
        // Initialize array copy and shuffle it
        fprintf (stdout, "Iteration %d: \n", i);
        for (int j=0; j<totalsize; ++j) w[j] = v[j];
        shuffle (w, totalsize);
        
        // Initialize papi events
        start = PAPI_get_real_usec();
        PAPI_start(EventSet);
        
        // Sort the array
        bucket_sort (w, totalsize);
        stop = PAPI_get_real_usec();

        // Stop papi events
        PAPI_stop(EventSet, metrics);

        // Check if array is sorted
        if (isOrdered(w, totalsize)) fprintf (stdout, "\tSUCCESS\n\n");
        else fprintf (stdout, "\tFAILURE\n\n");

        // Get metrics
        for (int i=0; i<NUM_EVENTS; ++i) means[i] += metrics[i];
        
        texe += (float) (stop - start);
    }

    // Calculate metric means
    for (int i=0; i<NUM_EVENTS; ++i) means[i] /= NUM_RUNS;
    texe /= NUM_RUNS;

    // Print mean metrics
    for (int i=0 ; i<NUM_EVENTS ; i++) {
        char EventCodeStr[PAPI_MAX_STR_LEN];
        
        if (PAPI_event_code_to_name(Events[i], EventCodeStr) == PAPI_OK)
            fprintf (stdout, "%s: %.2f\n", EventCodeStr, means[i]);
        else fprintf (stdout, "PAPI UNKNOWN EVENT: %.2f\n", means[i]);
    }
    fprintf (stdout, "Execution time: %.2f us\n", texe);
    fprintf (stdout, "CPI: %.3f\n", means[1]/means[0]);

    return 0;
}
