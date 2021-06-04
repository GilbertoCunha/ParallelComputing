#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>
#include "sorting.h"

#define NUM_RUNS 10 

double start1, start2, start3, totaltime, msgtime, proctime, waittime;

void bucket_sort (int v[], int tam, int num_buckets, int num_proc, int id) {
    start2 = MPI_Wtime();
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
    } 

    int *local, localsize;
    localsize = counts[id];
    local = malloc (localsize * sizeof (int));
    start2 = MPI_Wtime() - start2;

    // Split the array between the processes
    start3 = MPI_Wtime();
    MPI_Scatterv(v, counts, displacements, MPI_INT, local, localsize, MPI_INT, 0, MPI_COMM_WORLD);
    start3 = MPI_Wtime() - start3;

    // Order the array of each process
    proctime = MPI_Wtime();
    b = malloc (num_buckets * sizeof (bucket));
    for (i=0; i<num_buckets; ++i) {
        b[i].topo = 0;
        b[i].balde = malloc (localsize * sizeof(int));
    }
    distributeBuckets(b, local, localsize, num_buckets);
    for (i=0; i<num_buckets; ++i) 
        if (b[i].topo) quicksort (b[i].balde, b[i].topo);
    i=0;
    for (j=0; j<num_buckets; j++) {
        for (k=0; k<b[j].topo; k++)
            local[i++] = b[j].balde[k];
        free (b[j].balde);
    }
    free (b);
    proctime = MPI_Wtime() - proctime + start2;

    // Send message back to root process
    msgtime = MPI_Wtime();
    MPI_Gatherv(local, localsize, MPI_INT, v, counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);
    msgtime = MPI_Wtime() - msgtime + start3;
}

int main (int argc, char **argv) { 
    FILE *f; MPI_Status status;
    int totalsize = atoi(argv[1]);
    int num_buckets = atoi(argv[2]);
    int id, size, i, j, localsize, *w, *v, *local;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double msgtimes[size], proctimes[size], waittimes[size];
    
    if (id == 0) {
        // Open results csv file
        if (access("results.csv", F_OK) != 0) {
            f = fopen("results.csv", "w");
            fprintf (f, "TotalSize,NumBuckets,ProcessCount,");
            fprintf (f, "ProcTime,MsgTime,WaitTime,TotalTime\n"); 
        }
        else f = fopen("results.csv", "a");
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
        MPI_Barrier(MPI_COMM_WORLD);
        if (id == 0) start1 = MPI_Wtime();
        bucket_sort (w, totalsize, num_buckets, size, id);
        MPI_Gather(&proctime, 1, MPI_DOUBLE, proctimes, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Gather(&msgtime, 1, MPI_DOUBLE, msgtimes, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        //MPI_Barrier(MPI_COMM_WORLD);

        if (id == 0) {
            // Determine total time
            totaltime = MPI_Wtime() - start1;

            // Derive wait times for all processes
            proctime = mean(proctimes, size);
            msgtime = mean(msgtimes, size);
            waittime = totaltime - (proctime + msgtime);

            // Verify if array is ordered
            // if (isOrdered(w, totalsize)==1) printf ("SUCCESS\n");
            // else printf ("OUT OF ORDER\n");

            // Print arguments to csv file
            fprintf (f, "%d,", totalsize);
            fprintf (f, "%d,", num_buckets);
            fprintf (f, "%d,", size);

            // Log metrics to csv file
            fprintf (f, "%f,", proctime);
            fprintf (f, "%f,", msgtime);
            fprintf (f, "%f,", waittime);
            fprintf (f, "%f\n", totaltime); 
        } 
    }

    if (id == 0) fclose (f);
    MPI_Finalize();

    return 0;
}
