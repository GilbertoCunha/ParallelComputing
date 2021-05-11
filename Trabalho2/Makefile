totalsize = 10000000
num_buckets = 5
thread_count = 12
parallel = 1
sort_func = "quicksort"
cutoff = 100000

run: bucketSort.c
	@gcc -g -O2 -fopenmp -I/share/apps/papi/5.5.0/include -L/share/apps/papi/5.5.0/lib bucketSort.c sorting.c -lpapi -lm
	@sudo ./a.out $(totalsize) $(num_buckets) $(thread_count) $(parallel) $(sort_func) $(cutoff)

clean:
	@rm ./a.out
