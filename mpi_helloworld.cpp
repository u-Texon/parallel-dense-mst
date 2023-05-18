#include "mpi.h"

void MAX_DataSet(int *in, int *inout, int *len, MPI_Datatype *datatype);

int main(int argc, char **argv) {
    int procSize, procID, x, sum;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);
    MPI_Op myOp;
    MPI_Op_create((MPI_User_function *) MAX_DataSet, true, &myOp);

    if (procID == 0) {
        x = 15;
    } else {
        x = procID;
    }

    MPI_Reduce(&x, &sum, 1, MPI_INT, myOp, 0, MPI_COMM_WORLD);
    if (procID == 0) {
       // std::cout << sum << std::endl;
    }


    MPI_Finalize();
    return 0;
}

void MAX_DataSet(int *in, int *inout, int *len, MPI_Datatype *datatype) {
    for (int i = 0; i < *len; i++) {
        *inout = *in + *inout;
        in++;
        inout++;
    }
}