//Compilado em Linux.Sujeito a mudanças caso outro sistema seja utilizado.
 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
 
#define tam_bucket 100
#define num_bucket 10
#define max 10
 
typedef struct {
    int topo;
    int balde[tam_bucket];
} bucket;
 
//cabeçalho das funções
void bucket_sort (int v[],int tam);
void bubble (int v[],int tam);

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

void bucket_sort (int v[], int tam) {                                     
    bucket b[num_bucket];                                      
    int i,j,k;                                                 
    for(i=0; i<num_bucket; i++) b[i].topo=0; //inicializa todos os "topo"
         
    for(i=0; i<tam; i++) { //verifica em que balde o elemento deve ficar
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
         
    for(i=0; i<num_bucket; i++) //ordena os baldes
        if(b[i].topo) bubble(b[i].balde, b[i].topo);
         
    i=0;
    for(j=0; j<num_bucket; j++) { //põe os elementos dos baldes de volta no vetor
        for(k=0; k<b[j].topo; k++) {
            v[i]=b[j].balde[k];
            i++;
        }
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

int main () {
    int v[100];

    for (int i=0; i<100; ++i) v[i] = i;
    printf("Original array: ");
    for (int i = 0; i < 100; i++) printf("%d ", v[i]);
    
    shuffle (v, 100);
    printf("\nShuffled array: ");
    for (int i = 0; i < 100; i++) printf("%d ", v[i]);
    
    bucket_sort(v, 100);
    printf("\nSorted array: ");
    for (int i = 0; i < 100; i++) printf("%d ", v[i]);
    printf("\n");

    return 0;
}
