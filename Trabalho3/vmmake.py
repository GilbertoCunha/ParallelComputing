from itertools import product
from tqdm import tqdm
from os import system

# All execution arguments
# total_size = [10**i for i in range(3, 8)]
num_buckets = [i for i in range(1, 10)] + [i*10 for i in range(1, 10)] + [100*i for i in range(1, 11)] 
# thread_count = [i for i in range(1, 13)]
# cutoff = [10**i for i in range(1, 5)]

# Compile C program
system("gcc -g -O2 -fopenmp -I/share/apps/papi/5.5.0/include -L/share/apps/papi/5.5.0/lib bucketSort.c sorting.c -lpapi -lm")

# Execução quicksort em paralelo
"""ps = list(product(total_size, num_buckets, thread_count))
for s, b, c in ps:
    if b < s:
        command = f"./a.out {s} {b} {c} 1 \"quicksort\" 1"
        tqdm.write (command)
        system (command)
"""
# Execução quicksort sequencial
s = 10000000
for b in num_buckets: 
    if b < s: 
        command = f"./a.out {s} {b} 1 0 \"quicksort\" 1"
        tqdm.write (command)
        system (command)
