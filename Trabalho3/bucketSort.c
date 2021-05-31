#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>
#include "sorting.h"

// Papi defines
#define NUM_EVENTS 2
#define NUM_RUNS 10 


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


double start, time;

int main (int argc, char **argv) { 
    FILE *f; MPI_Status status;
    int totalsize = atoi(argv[1]);
    int num_buckets = atoi(argv[2]);
    int id, size, i, j, localsize, *w, *v;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (id == 0) {
        
        // Open results csv file
        if (access("buckets.csv", F_OK) != 0) {
            f = fopen("buckets.csv", "w");
            fprintf (f, "Total Size,Number of Buckets,Process Count,");
            fprintf (f, "Texe\n"); 
        }
        else f = fopen("buckets.csv", "a");

        // Create and shuffle array 
        v = malloc (totalsize * sizeof (int)); 
        for (i=0; i<totalsize; ++i) v[i] = i;

        // Make array copy
        w = malloc (totalsize * sizeof (int));
    }

    // Perform sorting computation
    for (i=0; i<NUM_RUNS; ++i) {
        if (id == 0) {
            // Initialize array copy and shuffle it
            for (j=0; j<totalsize; ++j) w[j] = v[j];
            shuffle (w, totalsize);
            
            start = MPI_Wtime();

            int sp = 0, cumsize = totalsize; 
            localsize = cumsize / size;
            cumsize -= localsize; sp += localsize;
            printf ("Start sendind data\n");
            for (j=1; j<size; ++j) {
                int psize = cumsize / (size - j);
                MPI_Send(&psize, 1, MPI_INT, j, j, MPI_COMM_WORLD);
                MPI_Send(w+sp, psize, MPI_INT, j, j, MPI_COMM_WORLD);
                cumsize -= psize;
                sp += psize;
            }
            printf ("Finished sendind data\n");
        } 
        
        // MPI_Barrier(MPI_COMM_WORLD);

        if (id != 0) {
            printf ("Started receiving data\n");
            MPI_Recv(&localsize, 1, MPI_INT, 0, id, MPI_COMM_WORLD, &status);
            w = malloc (localsize * sizeof (int));
            printf ("Received localsize\n");
            MPI_Recv(w, localsize, MPI_INT, 0, id, MPI_COMM_WORLD, &status);
            printf ("Finished receiving data");
        }
        
        bucket_sort (w, localsize, num_buckets);
        MPI_Barrier(MPI_COMM_WORLD);

        if (id == 0) {
            // Receive data to original array
            int sp = localsize, psize;
            for (j=1; j<size; ++j) {
                MPI_Recv(&psize, 1, MPI_INT, j, j, MPI_COMM_WORLD, &status);
                MPI_Recv(w+sp, psize, MPI_INT, j, j, MPI_COMM_WORLD, &status);
                sp += psize;
            }
            time = MPI_Wtime() - start;
            
            // Verify if array is ordered
            if (isOrdered(w, totalsize)==1) printf ("SUCCESS\n");

            // Print arguments to csv file
            fprintf (f, "%d,", totalsize);
            fprintf (f, "%d,", num_buckets);
            fprintf (f, "%d,", size);

            // Log metrics to csv file
            fprintf (f, "%f\n", time); 
        } else {
            MPI_Send(&localsize, 1, MPI_INT, 0, id, MPI_COMM_WORLD);
            MPI_Send(w, localsize, MPI_INT, 0, id, MPI_COMM_WORLD);
        }
    }

    if (id == 0) fclose (f);
    MPI_Finalize();

    return 0;
}
