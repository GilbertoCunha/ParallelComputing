from itertools import product
from tqdm import tqdm
from os import system
import numpy as np

# All execution arguments
total_size = [10**i for i in range(3, 8)]
num_buckets = [10**i for i in range(1, 7)]
thread_count = [i for i in range(1, 13)]
cutoff = [10**i for i in range(6)]

# Create grid of all combinations of arguments 
iterator = list(product(total_size, num_buckets, thread_count, cutoff))
print (f"Total number of runs: {len(iterator)}\n")

# Execução quicksort em paralelo
"""for s, b, c, cut in tqdm(iterator, desc="Parallel Quicksort", total=len(iterator)):
    if b < s and cut == cutoff[0]:
        command = f"make totalsize={s} num_buckets={b} thread_count={c} parallel=1 sort_func=\"quicksort\""
        tqdm.write (command)
        system (command)
"""

# Execução quicksort sequencial
for s, b, c, cut in tqdm(iterator, desc="Sequential Quicksort", total=len(iterator)):
    if b < s and c == thread_count[0] and cutoff == cutoff[0]:
        command = f"make totalsize={s} num_buckets={b} parallel=0 sort_func=\"quicksort\""
        tqdm.write (command)
        system (command)

# Execução quicksortparallel em paralelo
for s, b, c, cut in tqdm(iterator, desc="Parallel Quicksortparallel", total=len(iterator)):
    if b < s:
        command = f"make totalsize={s} num_buckets={b} thread_count={c} parallel=1 sort_func=\"quicksortparallel\" cutoff={cut}"
        tqdm.write (command)
        system (command)

# Execução quicksortparallel sequencial
for s, b, c, cut in tqdm(iterator, desc="Sequential QuicksortParallel", total=len(iterator)):
    if b < s and c == thread_count[0]:
        command = f"make totalsize={s} num_buckets={b} parallel=0 sort_func=\"quicksortparallel\" cutoff={cut}"
        tqdm.write (command)
        system (command)
