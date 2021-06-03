#include "sorting.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

double mean (double v[], int n) {
    double r = 0;

    for (int i=0; i<n; ++i) r += v[i];
    r /= n;

    return r;
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
        //printf("Thread %d executing\n", id);

        quicksortaux (arr, low, pi - 1);
        quicksortaux (arr, pi + 1, high);
    }
}

void quicksort (int v[], int tam) {
    quicksortaux (v, 0, tam-1);
}

void distributeBuckets (bucket b[], int v[], int tam, int num_bucket) {
    int i, j, aux, n;

    // Calcular máximo e mínimo do array 
    int max = v[0], min = v[0];
    for (i=1; i<tam; ++i) {
        if (v[i] > max) max = v[i];
        if (v[i] < min) min = v[i];
    }

    // Colocar cada elemento no balde correspondente
    n = ceil((double) (max - min + 1) / num_bucket);
    for (i=0; i<tam; i++) {
        aux = v[i];
        j = (aux - min) / n;
        b[j].balde[b[j].topo++] = aux;
    }
}
