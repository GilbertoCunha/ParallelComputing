from itertools import product
import numpy as np
from tqdm import tqdm
from os import system

def concatenate(L):
    r = []
    for l in L: r += l
    return r

# All execution arguments
total_size = concatenate([[10**j * i for i in range(2, 11)] for j in range(5, 7)])
# num_buckets = [i for i in range(1, 10)] + [10*i for i in range(1,10)] + [100*i for i in range(1, 11)]
#size_per_process = 1000000
#process_count = [i for i in range(1, 13)]
process_count = [1, 12]
num_buckets = [800]
#total_size = [p * size_per_process for p in process_count]

# Compile C program
system("mpicc bucketSort.c sorting.c -lm")

# Execução quicksort em paralelo
iterator = list(product(total_size, num_buckets, process_count))
for s, b, p in tqdm(iterator, desc="Gathering Data", total=len(iterator)):
    if b < s:
        command = f"mpirun --use-hwthread-cpus -np {p} ./a.out {s} {b}"
        tqdm.write (command)
        system (command)
"""
for s, p in tqdm(zip(total_size, process_count), desc="Gathering Data", total=len(total_size)):
    command = f"mpirun --use-hwthread-cpus -np {p} ./a.out {s} {800}"
    tqdm.write(command)
    system(command)
"""
