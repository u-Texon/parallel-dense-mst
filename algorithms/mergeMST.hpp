#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"


namespace mergeMST {


    inline WEdgeList getMST(int *n, const int *m, WEdge edges[]) {
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
                std::cout << "(" << edges[i].get_src() << "," << edges[i].get_dst() << "," << edges[i].get_weight()
                          << ") ";
            }
            std::cout << std::endl;
        } else {
            MPI_Recv(local, edgesPerProc, mapper.get_mpi_datatype(), 0, 0, MPI_COMM_WORLD, &status);
        }

        /*
        std::cout << "process " << ctx.rank() << " received :";
        for (int i = 0; i < edgesPerProc; i++) {
            std::cout << "(" << local[i].get_src() << "," << local[i].get_dst() << "," << local[i].get_weight() << ") ";
        }
        std::cout << std::endl;
    */


        WEdgeList localEdges;
        for (int i = 0; i < edgesPerProc; ++i) {
            localEdges.push_back(local[i]);
        }
        UnionFind uf(*n);
        WEdgeList mstList;
        mstList = filterKruskal::getMST(n, &localEdges, &uf);


        int j = 2;
        while (j <=cd  ctx.size()) {
            WEdge mst[mstList.size()];
            for (int i = 0; i < mstList.size(); ++i) {
                mst[i] = mstList.at(i);
            }

            int myMSTsize = (int) mstList.size();
            int otherMSTsize;
            WEdge otherMST[edgesPerProc]; //TODO: how do I initialize this correctly??

            MPI_Status s;
            int i = 0;
            while (i < ctx.size()) {
                if (ctx.rank() == i) {
                    MPI_Recv(&otherMSTsize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                    MPI_Recv(&otherMST, otherMSTsize, mapper.get_mpi_datatype(), i, 0, MPI_COMM_WORLD, &s);
                }
                if (ctx.rank() == i + j/2) {
                    MPI_Send(&myMSTsize, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
                    MPI_Send(mst, (int) mstList.size(), mapper.get_mpi_datatype(), i + 1, 0, MPI_COMM_WORLD);
                }
                i += j;
            }

            if (ctx.rank() % j == 0) {


                for (int x = 0; x < otherMSTsize; ++x) {
                    mstList.push_back(otherMST[i]);
                }

                UnionFind uf2(*n);
                mstList = filterKruskal::getMST(n, &mstList, &uf2);


                std::cout << ctx.rank() << " found mst:";
                for (auto &edge: mstList) {
                    std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
                }
                std::cout << std::endl;


            }

            j *= 2;
        }

        return mstList;
    }




} //namespace mergeMST