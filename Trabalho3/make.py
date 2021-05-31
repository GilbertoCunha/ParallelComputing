from itertools import product
from tqdm import tqdm
from os import system

# All execution arguments
total_size = [10**5 * i for i in range(2, 10)] + [10**6 * i for i in range(2, 10)]
# num_buckets = [i for i in range(1, 10)] + [10*i for i in range(1,10)] + [100*i for i in range(1, 11)]
# thread_count = [i for i in range(1, 13)]
# cutoff = [10**i for i in range(1, 5)]

# Compile C program
system("gcc -g -O2 -fopenmp -I/share/apps/papi/5.5.0/include -L/share/apps/papi/5.5.0/lib bucketSort.c sorting.c -lpapi -lm")

# Execução quicksort em paralelo
# ps = list(product(total_size, num_buckets, thread_count))
c = 12
b = 100
for s in tqdm(total_size, desc="Parallel Quicksort", total=len(total_size)):
    if b < s:
        command = f"sudo ./a.out {s} {b} {c} 1 \"quicksort\" 1"
        tqdm.write (command)
        system (command)

# Execução quicksort sequencial
# ss = list(product(total_size, num_buckets))
"""for s in tqdm(total_size, desc="Sequential Quicksort", total=len(total_size)):
    if b < s: 
        command = f"sudo ./a.out {s} {b} 1 0 \"quicksort\" 1"
        tqdm.write (command)
        system (command)
"""
# Execução quicksortparallel em paralelo
"""pp = list(product(total_size, num_buckets, thread_count, cutoff))
for s, b, c, cut in tqdm(pp, desc="Parallel Quicksortparallel", total=len(pp)):
    if b < s:
        command = f"sudo ./a.out {s} {b} {c} 1 \"quicksortparallel\" {cut}"
        tqdm.write (command)
        system (command)

# Execução quicksortparallel sequencial
sp = list(product(total_size, num_buckets, thread_count, cutoff))
for s, b, c, cut in tqdm(sp, desc="Sequential QuicksortParallel", total=len(sp)):
    if b < s:
        command = f"sudo ./a.out {s} {b} {c} 0 \"quicksortparallel\" {cut}"
        tqdm.write (command)
        system (command)
"""
