from itertools import product
from tqdm import tqdm
from os import system
import numpy as np

# All execution arguments
total_size = [10**i for i in range(3, 8)]
num_buckets = [10**i for i in range(1, 6)]
thread_count = [i for i in range(1, 13)]
cutoff = [10**i for i in range(5)]

# Execução quicksort em paralelo
ps = list(product(total_size, num_buckets, thread_count))
for s, b, c in tqdm(ps, desc="Parallel Quicksort", total=len(ps)):
    if b < s:
        command = f"make totalsize={s} num_buckets={b} thread_count={c} parallel=1 sort_func=\"quicksort\""
        tqdm.write (command)
        system (command)

# Execução quicksort sequencial
ss = list(product(total_size, num_buckets))
for s, b in tqdm(ss, desc="Sequential Quicksort", total=len(ss)):
    if b < s: 
        command = f"make totalsize={s} num_buckets={b} parallel=0 sort_func=\"quicksort\""
        tqdm.write (command)
        system (command)

# Execução quicksortparallel em paralelo
pp = list(product(total_size, num_buckets, thread_count, cutoff))
for s, b, c, cut in tqdm(pp, desc="Parallel Quicksortparallel", total=len(pp)):
    if b < s:
        command = f"make totalsize={s} num_buckets={b} thread_count={c} parallel=1 sort_func=\"quicksortparallel\" cutoff={cut}"
        tqdm.write (command)
        system (command)

# Execução quicksortparallel sequencial
sp = list(product(total_size, num_buckets, cutoff))
for s, b, cut in tqdm(sp, desc="Sequential QuicksortParallel", total=len(sp):
    if b < s:
        command = f"make totalsize={s} num_buckets={b} parallel=0 sort_func=\"quicksortparallel\" cutoff={cut}"
        tqdm.write (command)
        system (command)
