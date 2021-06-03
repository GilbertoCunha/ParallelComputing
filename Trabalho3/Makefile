totalsize = 1000000
np = 4 

run: bucketSort.c sorting.c
	@mpicc bucketSort.c sorting.c -lm
	@mpirun -np $(np) ./a.out $(totalsize)

clean:
	@rm ./a.out
