//Compilado em Linux.Sujeito a mudanças caso outro sistema seja utilizado.
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
 
#define tam_bucket 10000 // Tamanho total do array
#define num_bucket 10 // Número de baldes
#define max 1000000 // Maior número que pode ser ordenado
 
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
 
//cabeçalho das funções
void bucket_sort (int v[], int tam);
void bubble (int v[],int tam);
void bubble1 (int v[],int tam);
void bubble2 (int v[],int tam);
void distributeBuckets (bucket b[], int v[], int tam);
void distributeBuckets1 (bucket b[], int v[], int tam);

void shuffle (int array[], int n) {
    if (n > 1) {
        int i;
        for (i = 0; i < n - 1; i++) {
            int j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int isOrdered (int v[], int size) {
    int i;
    for (i=1; i<size && v[i-1]<=v[i]; ++i);
    return i == size;
}

void bucket_sort (int v[], int tam) {                                     
    bucket b[num_bucket];                                      
    int i, j, k, aux;                                                 
    for(i=0; i<num_bucket; i++) {
        b[i].topo=0; // Inicializa todos os "topo"
        b[i].balde = malloc (num_bucket * tam_bucket * sizeof(int)); // Inicializar o array com o tamanho adequado
    }
         
    // Distribuir os elementos do array pelos baldes
    // distributeBuckets (b, v, tam);
    distributeBuckets1 (b, v, tam);
         
    for(i=0; i<num_bucket; i++) //ordena os baldes
        if(b[i].topo) bubble2 (b[i].balde, b[i].topo);
         
    i=0;
    for(j=0; j<num_bucket; j++) { //põe os elementos dos baldes de volta no vetor
        for(k=0; k<b[j].topo; k++) {
            v[i]=b[j].balde[k];
            i++;
        }
    }
}

void distributeBuckets (bucket b[], int v[], int tam) {
    int i, j;
    for(i=0; i<tam; i++) {
        j=(num_bucket)-1;
        while (1) {
            if(j<0) break;
            if(v[i]>=j*max) {
                b[j].balde[b[j].topo]=v[i];
                (b[j].topo)++;
                break;
            }
            j--;
        }
    }
}

void distributeBuckets1 (bucket b[], int v[], int tam) {
    int i, j, aux;
    for(i=0; i<tam; i++) {
        aux = v[i];
        j = aux / (max / num_bucket);
        b[j].balde[b[j].topo++] = aux;
    }
}
 
void bubble (int v[], int tam) {
    int i,j,temp,flag;
    if (tam) {
        for(j=0; j<tam-1; j++) {
            flag=0;
            for(i=0; i<tam-1; i++) {
                if(v[i+1]<v[i]) {
                    temp=v[i];
                    v[i]=v[i+1];
                    v[i+1]=temp;
                    flag=1;
                }
            }
            if(!flag) break;
        }
    }
}

void bubble1 (int v[], int tam) {
    int i,j,temp,flag;

    for(j=0; j<tam-1; j++) {
        flag=0;
        for(i=0; i<tam-j-1; i++) {
            if(v[i+1]<v[i]) {
                temp=v[i];
                v[i]=v[i+1];
                v[i+1]=temp;
                flag=1;
            }
        }
        if(!flag) break;
    }
}

void bubble2 (int v[], int tam) {
    int i,j,temp,flag;

    for(j=0; j<tam-1; j++) {
        flag=0;
        for(i=0; i<tam-j-1; i++) {
            if(v[i]>v[i+1]) {
                temp=v[i];
                v[i]=v[i+1];
                v[i+1]=temp;
                flag=1;
            }
        }
        if(!flag) break;
    }
}

#include "papi.h"
#define NUM_EVENTS 2
#define NUM_RUNS 5

// Add papi metrics
long long metrics[NUM_EVENTS];
long long start, stop;
float texe = 0;
float means[NUM_EVENTS];
int Events[NUM_EVENTS] = { PAPI_TOT_INS, PAPI_TOT_CYC };
int EventSet = PAPI_NULL;

int main () {

    // Create and shuffle array
    int totalsize = tam_bucket * num_bucket;
    int v[totalsize];
    for (int i=0; i<totalsize; ++i) v[i] = i%max;
    shuffle (v, totalsize);

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
        printf ("Iteration %d: ", i);
        for (int j=0; j<totalsize; ++j) w[j] = v[j];
        
        start = PAPI_get_real_usec();
        PAPI_start(EventSet);
        
        bucket_sort(w, totalsize);

        PAPI_stop(EventSet, metrics);
        if (isOrdered(w, totalsize)) printf ("success\n");
        else printf ("failure\n");
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
            printf ("%s = %.2f\n", EventCodeStr, means[i]);
        else printf ("PAPI UNKNOWN EVENT = %.2f\n", means[i]);
    }
    printf ("Execution time: %.2f us\n", texe);

    return 0;
}
