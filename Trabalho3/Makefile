totalsize = 1000000
np = 4 
num_buckets = 10

run: bucketSort.c sorting.c
	@mpicc bucketSort.c sorting.c -lm
	@mpirun --use-hwthread-cpus -np $(np) ./a.out $(totalsize) $(num_buckets) 

clean:
	@rm ./a.out
