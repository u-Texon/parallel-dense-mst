#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"

//TODO: view boost parallel graph library
namespace mergeMST {


    inline WEdgeList getMST(int *n, const int *m, WEdgeList edges) {
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally
        hybridMST::mpi::TypeMapper<WEdge> mapper;

        int edgesPerProc = *m / ctx.size();
        int rest = *m % ctx.size();
        assert(edgesPerProc * ctx.size() + rest == edges.size());
        WEdgeList localEdges;


        MPI_Status status;

        if (ctx.rank() == 0) {
            for (int i = 0; i < edgesPerProc; ++i) {
                localEdges.push_back(edges[i]);
            }
            for (int i = 1; i < ctx.size() - 1; ++i) {
                MPI_Send(edges.data() + i * edgesPerProc, edgesPerProc, mapper.get_mpi_datatype(), i, 0,
                         MPI_COMM_WORLD);
            }
            MPI_Send(edges.data() + (ctx.size() -1) * edgesPerProc, edgesPerProc + rest, mapper.get_mpi_datatype(),
                     ctx.size() - 1, 0,
                     MPI_COMM_WORLD);
    /*
            std::cout << "0 sent: ";
            for (auto &edge: edges) {
                std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
            }
            std::cout << std::endl;
            */

        } else {
            if (ctx.rank() == ctx.size() - 1) {
                edgesPerProc += rest;
            }
            localEdges.resize(edgesPerProc);
            MPI_Recv(localEdges.data(), edgesPerProc, mapper.get_mpi_datatype(), 0, 0, MPI_COMM_WORLD, &status);
            //   std::cout << "process " << ctx.rank() << " received!" << std::endl;
        }

        /*
        std::cout << ctx.rank() << " got: ";
        for (auto &edge: localEdges) {
            std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        }
        std::cout << std::endl;
        */

        UnionFind uf(*n);
        WEdgeList mstList;
        mstList = filterKruskal::getMST(n, &localEdges, &uf);

        int j = 2;
        while (j <= ctx.size()) { //is this fine?
            int myMSTsize = (int) mstList.size();
            int otherMSTsize = 0;
            int i = 0;
            while (i < ctx.size()) {
                if (ctx.rank() == i) {
                    MPI_Recv(&otherMSTsize, 1, MPI_INT, i + j / 2, 0, MPI_COMM_WORLD, &status);
                } else if (ctx.rank() == i + j / 2) {
                    MPI_Send(&myMSTsize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                }
                i += j;
            }
            WEdgeList otherMST;
            otherMST.resize(otherMSTsize);


            i = 0;
            while (i < ctx.size()) {
                if (ctx.rank() == i) {
                    MPI_Recv(otherMST.data(), otherMSTsize, mapper.get_mpi_datatype(), i + j / 2, 0, MPI_COMM_WORLD,
                             &status);
                    break;
                } else if (ctx.rank() == i + j / 2) {
                    MPI_Send(mstList.data(), (int) mstList.size(), mapper.get_mpi_datatype(), i, 0, MPI_COMM_WORLD);
                    break;
                }
                i += j;
            }

            if (ctx.rank() % j == 0) {
                for (auto edge: otherMST) {
                    mstList.push_back(edge);
                }
                uf.clear(); //TODO: is it even possible to keep the UF??
                mstList = filterKruskal::getMST(n, &mstList, &uf);
            }
            j *= 2;
        }
        return mstList;
    }


} //namespace mergeMST