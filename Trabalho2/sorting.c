#include "sorting.h"
#include "limits.h"
#include "stdlib.h"
#include <stdio.h>

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

void swap (int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition (int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1), j;
    for (j = low; j <= high-1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksortaux (int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        int id = omp_get_thread_num();
        //printf("Thread %d executing\n", id);

        quicksortaux (arr, low, pi - 1);
        quicksortaux (arr, pi + 1, high);
    }
}

void quicksort (int v[], int tam) {
    quicksortaux (v, 0, tam-1);
}

void distributeBuckets (bucket b[], int v[], int tam, int num_bucket) {
    int i, j, aux;
    float n;

    // Calcular máximo e mínimo do array 
    int max = v[0], min = v[0];
    for (i=1; i<tam; ++i) {
        if (v[i] > max) max = v[i];
        if (v[i] < min) min = v[i];
    }

    // Colocar cada elemento no balde correspondente
    n = (max - min + 1) / num_bucket;
    for (i=0; i<tam; i++) {
        aux = v[i];
        j = aux / n;
        b[j].balde[b[j].topo++] = aux;
    }
}

void distributeBucketsParallel (bucket b[], int v[], int tam, int num_bucket, int thread_count) {
    int i, j, k;
    float n;

    int sizes[thread_count+1], cum_size=tam;
    sizes[0] = 0;
    for (i=1; i<thread_count; ++i) {
        sizes[i] = sizes[i-1] + cum_size / (thread_count-i+1);
        cum_size -= sizes[i] - sizes[i-1]; 
    }
    sizes[thread_count] = tam;

    // Calcular máximo e mínimo em pedaços do array para cada thread 
    int maxs[thread_count], mins[thread_count];
    #pragma omp parallel num_threads(thread_count)
    {
        #pragma omp for schedule(dynamic)
        for(i=0; i<thread_count; ++i) {
            int max = v[sizes[i]], min = v[sizes[i]];
            for (j=sizes[i]+1; j<sizes[i+1]; ++j) {
                if (v[j] > max) max = v[j];
                if (v[j] < min) min = v[j];
            }
            maxs[i] = max;
            mins[i] = min;
        }
    }

    int max = maxs[0], min = mins[0];
    for (i=1; i<thread_count; ++i) {
        if (max < maxs[i]) max = maxs[i];
        if (min > mins[i]) min = mins[i];
    }

    // Inicializar as locks
    for (i=0; i<num_bucket; ++i)
       omp_init_lock(&(b[i].lock));

    // Colocar cada elemento no balde correspondente
    n = (max - min + 1) / num_bucket;
    #pragma omp parallel num_threads(thread_count)
    {
        int iaux;
        #pragma omp for schedule(dynamic)
        for (k=0; k<thread_count; ++k) {
            for (iaux=sizes[k]; iaux<sizes[k+1]; iaux++) {
                int aux = v[iaux];
                int w = aux / n;
                omp_set_lock(&(b[w].lock));
                b[w].balde[b[w].topo++] = aux;
                omp_unset_lock(&(b[w].lock));
            }
        }
    }
    
    for (i=0; i<num_bucket; ++i)
       omp_destroy_lock(&(b[i].lock));
}
