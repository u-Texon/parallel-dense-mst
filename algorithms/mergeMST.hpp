#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"


namespace mergeMST {


    inline WEdge getMST(int *n, int *m, WEdge edges[]) {
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally


        int edgesPerProc = *m / ctx.size();
        //TODO: int rest = *m % ctx.size();
        WEdge local[edgesPerProc];
        hybridMST::mpi::TypeMapper<WEdge> mapper;

        MPI_Status status;

        if (ctx.rank() == 0) {
            for (int i = 0; i < edgesPerProc; ++i) {
                local[i] = edges[i];
            }
            for (int i = 1; i < ctx.size(); ++i) {
                MPI_Send(edges + (i) * edgesPerProc, edgesPerProc, mapper.get_mpi_datatype(), i, 0, MPI_COMM_WORLD);
            }
            for (int i = 0; i < *m; i++) {
                std::cout << "(" << edges[i].get_src() << "," << edges[i].get_dst() << "," << edges[i].get_weight() << ") ";
            }
            std::cout << std::endl;
        } else {
            MPI_Recv(local, edgesPerProc, mapper.get_mpi_datatype(), 0, 0, MPI_COMM_WORLD, &status);
        }

        std::cout << "process " << ctx.rank() << " received :";
        for (int i = 0; i < edgesPerProc; i++) {
            std::cout << "(" << local[i].get_src() << "," << local[i].get_dst() << "," << local[i].get_weight() << ") ";
        }
        std::cout << std::endl;

        /*
        WEdgeList *localEdges;
        for (int i = 0; i < edgesPerProc; ++i) {
            localEdges->push_back(local[i]);
        }
        UnionFind uf(*n);
        WEdgeList *mst;
        *mst = filterKruskal::filter(localEdges, &uf);
        */
        return edges[0];
    }

    inline void helloWorld() {
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally

        int rank = ctx.rank();
        int size = ctx.size();

        std::cout << "Hello World! This is node " << rank + 1 << " of " << size << std::endl;
    }


} //namespace mergeMST