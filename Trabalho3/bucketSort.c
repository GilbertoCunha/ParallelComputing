#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>
#include "sorting.h"

#define NUM_RUNS 1 

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
    int id, size, i, j, localsize, *w, *v, *local;

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

        // Create array 
        v = malloc (totalsize * sizeof (int)); 
        for (i=0; i<totalsize; ++i) v[i] = i;

        // Allocate memory for array copy
        w = malloc (totalsize * sizeof (int));
    }

    // Perform sorting computation
    for (i=0; i<NUM_RUNS; ++i) {
                
        start = MPI_Wtime();

        // Determine localsize
        int sp = 0;
        int counts[size], displacements[size];
        
        // Send array sizes to processes and store them in an array
        for (j=0; j<size; ++j) {
            displacements[j] = sp;
            counts[j] = (totalsize - sp) / (size - j);
            sp += counts[j];
        }
        localsize = counts[id];
        local = malloc (localsize * sizeof (int));
        
        if (id == 0) {
            // Initialize array copy and shuffle it
            for (j=0; j<totalsize; ++j) w[j] = v[j];
            shuffle (w, totalsize);

            printf ("Original array: ");
            for (j=0; j<totalsize; ++j) printf ("%d ", w[j]);
            printf ("\n");

            MPI_Scatterv(w, counts, displacements, MPI_INT, local, localsize, MPI_INT, 0, MPI_COMM_WORLD);
        } else 
            MPI_Scatterv(NULL, NULL, NULL, MPI_INT, local, localsize, MPI_INT, 0, MPI_COMM_WORLD);
        
        bucket_sort (local, localsize, num_buckets);
        // if (isOrdered(local, localsize)) printf ("Process %d sorted successfully\n", id);
        // else printf ("Process %d did not sort\n", id);
        MPI_Barrier(MPI_COMM_WORLD);

        printf ("Process %d sorted array: ", id);
        for (j=0; j<localsize; ++j) printf ("%d ", local[j]);
        printf ("\n");

        // Receive data again in root process
        MPI_Gatherv(local, localsize, MPI_INT, w, counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);

        if (id == 0) {
            time = MPI_Wtime() - start;

            printf ("Final array: ");
            for (j=0; j<totalsize; ++j) printf ("%d ", w[j]);
            printf ("\n");
            
            // Verify if array is ordered
            // if (isOrdered(w, totalsize)==1) printf ("SUCCESS\n");
            // else printf ("OUT OF ORDER\n");

            // Print arguments to csv file
            fprintf (f, "%d,", totalsize);
            fprintf (f, "%d,", num_buckets);
            fprintf (f, "%d,", size);

            // Log metrics to csv file
            fprintf (f, "%f\n", time); 
        } 
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (id == 0) fclose (f);
    MPI_Finalize();

    return 0;
}
