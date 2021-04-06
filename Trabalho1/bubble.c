#include "bubble.h"
#include "limits.h"
#include "stdlib.h"

// #pragma GCC target("arch=znver2")
// #pragma GCC optimize("tree-vectorize")

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
                temp = v[i];
                v[i] = v[i+1];
                v[i+1] = temp;
                flag = 1;
            }
        }
        if(!flag) break;
    }
}

void bubble3 (int * __restrict__ v, int tam) {
    int i, j, temp, flag;

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

void mergesort (int v[], int tam) {
    if (tam > 1) {
        int mid = tam / 2;
        // Divide
        mergesort (v, mid);
        mergesort (v + mid, tam - mid);
        // Conquer
        merge (v, tam, mid);
    }
}

void distributeBuckets (bucket b[], int v[], int tam, int num_bucket) {
    int i, j;

    // Calcular máximo e mínimo do array
    int max = v[0], min = v[0];
    for (i=1; i<tam; ++i) {
        if (v[i] > max) max = v[i];
        if (v[i] < min) min = v[i];
    }

    // Colocar cada elemento no balde correspondente
    for(i=0; i<tam; i++) {
        j=num_bucket-1;
        while (1) {
            if(j<0) break;
            if(v[i]>=j * ((max - min + 1) / num_bucket)) {
                b[j].balde[b[j].topo]=v[i];
                (b[j].topo)++;
                break;
            }
            j--;
        }
    }
}

void distributeBuckets1 (bucket b[], int v[], int tam, int num_bucket) {
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
    for(i=0; i<tam; i++) {
        aux = v[i];
        j = aux / n;
        b[j].balde[b[j].topo++] = aux;
    }
}

void bucket_sort1 (int * __restrict__ v, int tam, int num_bucket) {                                     
    bucket b[num_bucket];                                      
    int i, j, k;

    // Inicializar os baldes                                                 
    for(i=0; i<num_bucket; i++) {
        b[i].topo = 0; // Inicializar o número de elementos usados no balde
        b[i].balde = malloc (tam * sizeof(int)); // Inicializar o array com o tamanho adequado
    }
         
    // Distribuir os elementos do array pelos baldes
    distributeBuckets1 (b, v, tam, num_bucket);
         
    // Ordenar os baldes
    for(i=0; i<num_bucket; i++)
        if(b[i].topo) 
            bubble3 (b[i].balde, b[i].topo);
         
    // Inserir os elementos ordenados dos baldes de volta no vetor
    i=0;
    for(j=0; j<num_bucket; j++)
        for(k=0; k<b[j].topo; k++)
            v[i++] = b[j].balde[k];
}