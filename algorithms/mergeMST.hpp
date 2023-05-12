#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"

namespace mergeMST {

    inline WEdgeList getMST(int &n, WEdgeList &edges) {
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally
        hybridMST::mpi::TypeMapper<WEdge> mapper;

        // calculate local MST with filter kruskal
        UnionFind uf(n);
        WEdgeList mstList;
        mstList = filterKruskal::getMST(n, edges, uf);


        int j = 2;
        while (j <= ctx.size()) {
            int myMSTsize = (int) mstList.size();
            int otherMSTsize = 0;
            int i = 0;
            while (i < ctx.size()) {   // send local mst size to other processor
                if (ctx.rank() == i) {
                    MPI_Status status;
                    MPI_Recv(&otherMSTsize, 1, MPI_INT, i + j / 2, 0, ctx.communicator(), &status);
                } else if (ctx.rank() == i + j / 2) {
                    MPI_Send(&myMSTsize, 1, MPI_INT, i, 0, ctx.communicator());
                }
                i += j;
            }
            WEdgeList otherMST;
            otherMST.resize(otherMSTsize);


            i = 0;
            while (i < ctx.size()) {   // send local mst to other processor
                if (ctx.rank() == i) {
                    MPI_Status status;
                    MPI_Recv(otherMST.data(), otherMSTsize, mapper.get_mpi_datatype(), i + j / 2, 0, ctx.communicator(),
                             &status);
                    break;
                } else if (ctx.rank() == i + j / 2) {
                    MPI_Send(mstList.data(), (int) mstList.size(), mapper.get_mpi_datatype(), i, 0, ctx.communicator());
                    break;
                }
                i += j;
            }

            //calculate new mst
            if (ctx.rank() % j == 0) {
                for (auto edge: otherMST) {
                    mstList.push_back(edge);
                }
                uf.clear();
                mstList = filterKruskal::getMST(n, mstList, uf);
            }
            j *= 2;
        }
        return mstList;
    }


} //namespace mergeMST