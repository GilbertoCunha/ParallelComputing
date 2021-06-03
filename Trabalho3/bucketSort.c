#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>
#include "sorting.h"

#define NUM_RUNS 10 

void bucket_sort (int v[], int tam, int num_proc, int id) {
    bucket *b = malloc (num_proc * sizeof (bucket));                                      
    int i, j, k;

    // Inicializar os baldes
    for(i=0; i<num_proc; i++) {
        b[i].topo = 0; // Inicializar o número de elementos usados no balde
        b[i].balde = malloc (tam * sizeof(int)); // Inicializar o array com o tamanho adequado
    }

    // Distribuir os elementos do array pelos baldes
    distributeBuckets (b, v, tam, num_proc);

    // Colocar os baldes de novo no array original
    int counts[num_proc], displacements[num_proc], sp = 0;
    for (i=0; i<num_proc; ++i) {
        counts[i] = b[i].topo;
        displacements[i] = sp;
        for (j=0; j<b[i].topo; ++j) v[sp+j] = b[i].balde[j];
        sp += counts[i];
        free (b[i].balde);
    } 
    free (b);

    int *local, localsize;
    localsize = counts[id];
    local = malloc (localsize * sizeof (int));

    MPI_Scatterv(v, counts, displacements, MPI_INT, local, localsize, MPI_INT, 0, MPI_COMM_WORLD);
    quicksort (local, localsize);
    MPI_Gatherv(local, localsize, MPI_INT, v, counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);
}


double start, time;

int main (int argc, char **argv) { 
    FILE *f; MPI_Status status;
    int totalsize = atoi(argv[1]);
    // int num_buckets = atoi(argv[2]);
    int id, size, i, j, localsize, *w, *v, *local;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (id == 0) {
        // Open results csv file
        if (access("buckets.csv", F_OK) != 0) {
            f = fopen("buckets.csv", "w");
            fprintf (f, "Total Size,Process Count,");
            fprintf (f, "Texe\n"); 
        }
        else f = fopen("buckets.csv", "a");
    }

    // Create array 
    v = malloc (totalsize * sizeof (int)); 
    for (i=0; i<totalsize; ++i) v[i] = i;

    // Allocate memory for array copy
    w = malloc (totalsize * sizeof (int));

    // Perform sorting computation
    for (i=0; i<NUM_RUNS; ++i) {

        // Initialize array copy and shuffle it
        for (j=0; j<totalsize; ++j) w[j] = v[j];
        shuffle (w, totalsize);

        // Order array
        if (id == 0) start = MPI_Wtime();
        bucket_sort (w, totalsize, size, id);

        if (id == 0) {
            time = MPI_Wtime() - start;

            // Verify if array is ordered
            if (isOrdered(w, totalsize)==1) printf ("SUCCESS\n");
            else printf ("OUT OF ORDER\n");

            // Print arguments to csv file
            fprintf (f, "%d,", totalsize);
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
