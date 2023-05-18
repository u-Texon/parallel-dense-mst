#include "mpi.h"
#include "include/mpi/context.hpp"
#include "include/mpi/type_handling.hpp"
#include "include/definitions.hpp"
#include <iostream>

void MAX_DataSet(WEdge *in, WEdge *inout, const int *n, MPI_Datatype *datatype) {
    for (int i = 0; i < *n; i++) {
        if (in[i].get_weight() < inout[i].get_weight()) {
            inout[i] = in[i];
        }
    }
}

int main(int argc, char **argv) {
    hybridMST::mpi::MPIContext ctx;
    MPI_Op myOp;
    MPI_Op_create((MPI_User_function *) MAX_DataSet, true, &myOp);

    hybridMST::mpi::TypeMapper<WEdge> mapper;

    WEdgeList edges;
    WEdgeList minimalEdges;
    minimalEdges.resize(2);
    if (ctx.rank() == 0) {
        edges.push_back(WEdge(0,1,10));
        edges.push_back(WEdge(0,2,1));
    } else {
        edges.push_back(WEdge(ctx.rank(),1,2 * ctx.rank()));
        edges.push_back(WEdge(ctx.rank(),2,2 * ctx.rank()));
    }

    MPI_Reduce(edges.data(), minimalEdges.data(), 2, mapper.get_mpi_datatype(), myOp, 0, MPI_COMM_WORLD);
    if (ctx.rank() == 0) {
       for (auto &edge: minimalEdges) {
           std::cout << edge << std::endl;
       }
    }


    return 0;
}

