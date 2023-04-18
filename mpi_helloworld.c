#include <stdio.h>
#include <mpi.h>

int main(int argc, char** args) {
int rank, size;
MPI_Init(&argc, &args);
MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
printf("Hello World! This is node %d of %d.\n", rank + 1, size);
MPI_Finalize();
return 0;
}