#include "bubble.h"
#include "limits.h"
#include "stdlib.h"
#include <omp.h>

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

void bubblesort (int v[], int tam) {
    int i,j,temp,flag;

    for(j=0; j<tam-1; j++) {
        flag=0;
        for(i=0; i<tam-j-1; i++) {
            if(v[i]>v[i+1]) {
                temp = v[i];
                v[i] = v[i+1];
                v[i+1] = temp;
                flag = 1;
            }
        }
        if(!flag) break;
    }
}

void merge (int v[], int n, int mid) {
    int *a = malloc (mid * sizeof (int));
    int *b = malloc ((n-mid) * sizeof (int));

    // Subdivide array
    for (int i=0; i<mid; ++i) a[i] = v[i];
    for (int i=0; i<n-mid; ++i) b[i] = v[mid+i];

    // Insert elements of arrays a and b into v in sorted manner
    int i=0, j=0, k=0;
    while (j<mid && k<n-mid) {
        if (a[j] < b[k]) v[i++] = a[j++];
        else v[i++] = b[k++];
    }

    // Insert leftover elements
    while (j<mid) v[i++] = a[j++];
    while (k<n-mid) v[i++] = b[k++];
}

void mergesortaux (int v[], int tam, int cutoff) {
    if (tam > 1) {
        int mid = tam / 2;
        // Divide
        if (tam < cutoff) {
            mergesortaux (v, mid, cutoff);
            mergesortaux (v + mid, tam - mid, cutoff);
        }
        else {
            #pragma omp parallel sections
            {
                #pragma omp section
                { mergesortaux (v, mid, cutoff); }
                #pragma omp section
                { mergesortaux (v + mid, tam - mid, cutoff); }
            }
        }
        // Conquer
        merge (v, tam, mid);
    }
}

void mergesort (int v[], int tam) {
    mergesortaux (v, tam, 1000);
}

void swap (int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition (int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high-1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksortaux (int arr[], int low, int high, int cutoff) {
    if (low < high) {
        int pi = partition(arr, low, high);

        if (high - low < cutoff) {
            quicksortaux (arr, low, pi - 1, cutoff);
            quicksortaux (arr, pi + 1, high, cutoff);
        }
        else {
            #pragma omp parallel sections 
            {
                #pragma omp section 
                { quicksortaux (arr, low, pi - 1, cutoff); }
                #pragma omp section 
                { quicksortaux (arr, pi + 1, high, cutoff); }
            }
        }
    }
}

void quicksort (int v[], int tam) {
    // #pragma omp single nowait
    quicksortaux (v, 0, tam-1, 1000);
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
