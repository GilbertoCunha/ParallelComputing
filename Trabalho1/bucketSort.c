#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "papi.h"
#include "bubble.h"

// Papi defines
#define NUM_EVENTS 2
#define NUM_RUNS 5
 
// Tamanho total do array
#define totalsize 1000000

// BucketSort defines
#define num_bucket 100000 // Número de baldes
#define tam_bucket (int) totalsize/num_bucket // Tamanho total de cada balde
 
//cabeçalho das funções
// void bucket_sort (int v[], int tam);

void bucket_sort (int v[], int tam) {                                     
    bucket *b = malloc (num_bucket * sizeof (bucket));                                      
    int i, j, k;

    // Inicializar os baldes                                                 
    for(i=0; i<num_bucket; i++) {
        b[i].topo = 0; // Inicializar o número de elementos usados no balde
        b[i].balde = malloc (tam * sizeof(int)); // Inicializar o array com o tamanho adequado
    }
         
    // Distribuir os elementos do array pelos baldes
    // distributeBuckets (b, v, tam, num_bucket);
    distributeBuckets1 (b, v, tam, num_bucket);
         
    // Ordenar os baldes
    for(i=0; i<num_bucket; i++)
        if(b[i].topo) {
            // bubble (b[i].balde, b[i].topo);
            // bubble1 (b[i].balde, b[i].topo);
            // bubble2 (b[i].balde, b[i].topo);
            // bubble3 (b[i].balde, b[i].topo);
            mergesort (b[i].balde, b[i].topo);
        }
         
    // Inserir os elementos ordenados dos baldes de volta no vetor
    i=0;
    for(j=0; j<num_bucket; j++)
        for(k=0; k<b[j].topo; k++)
            v[i++] = b[j].balde[k];
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
    int v[totalsize];
    for (int i=0; i<totalsize; ++i) v[i] = i;

    // Make array copy
    int w[totalsize];
    
    // Start papi counters
    // PAPI_start_counters(Events, NUM_EVENTS);
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);
    PAPI_add_events(EventSet, Events, NUM_EVENTS);
    for (int i=0; i<NUM_EVENTS; ++i) means[i] = 0;

    // Perform sorting computation
    for (int i=0; i<NUM_RUNS; ++i) {
        fprintf (stdout, "Iteration %d: ", i);
        for (int j=0; j<totalsize; ++j) w[j] = v[j];
        shuffle (w, totalsize);
        
        start = PAPI_get_real_usec();
        PAPI_start(EventSet);
        
        bucket_sort (w, totalsize);
        // bucket_sort1 (w, totalsize, num_bucket);

        PAPI_stop(EventSet, metrics);
        if (isOrdered(w, totalsize)) fprintf (stdout, "success\n");
        else fprintf (stdout, "failure\n");
        for (int i=0; i<NUM_EVENTS; ++i) means[i] += metrics[i];
        
        texe += (float) (PAPI_get_real_usec() - start);
    }

    // Calculate metric means
    for (int i=0; i<NUM_EVENTS; ++i) means[i] /= NUM_RUNS;
    texe /= NUM_RUNS;

    // Print mean metrics
    for (int i=0 ; i<NUM_EVENTS ; i++) {
        char EventCodeStr[PAPI_MAX_STR_LEN];
        
        if (PAPI_event_code_to_name(Events[i], EventCodeStr) == PAPI_OK)
            fprintf (stdout, "%s = %.2f\n", EventCodeStr, means[i]);
        else fprintf (stdout, "PAPI UNKNOWN EVENT = %.2f\n", means[i]);
    }
    fprintf (stdout, "Execution time: %.2f us\n", texe);
    fprintf (stdout, "CPI: %.3f\n", means[1]/means[0]);

    return 0;
}
