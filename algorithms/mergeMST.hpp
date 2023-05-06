#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"

namespace mergeMST {


    inline WEdgeList getMST(int *n, WEdgeList edges) {
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally
        hybridMST::mpi::TypeMapper<WEdge> mapper;

        int edgesPerProc = (int) edges.size() / ctx.size();
        int rest = (int) edges.size() % ctx.size();
        int bigProcs = ctx.size() - rest;
        assert(edgesPerProc * ctx.size() + rest == edges.size());
        WEdgeList localEdges;


        MPI_Status status;

        //scatter edges over all processes
        if (ctx.rank() == 0) {
            for (int i = 0; i < edgesPerProc; ++i) {
                localEdges.push_back(edges[i]);
            }
            for (int i = 1; i < bigProcs; ++i) {
                MPI_Send(edges.data() + i * edgesPerProc, edgesPerProc, mapper.get_mpi_datatype(), i, 0,
                         MPI_COMM_WORLD);
            }
            int amount = edgesPerProc + 1;
            for (int i = bigProcs; i < ctx.size(); ++i) {
                MPI_Send(edges.data() + bigProcs * edgesPerProc + (i - bigProcs) * amount, amount,
                         mapper.get_mpi_datatype(), i, 0,
                         MPI_COMM_WORLD);
            }

        } else {
            if (ctx.rank() >= bigProcs) {
                edgesPerProc += 1;
            }
            localEdges.resize(edgesPerProc);
            MPI_Recv(localEdges.data(), edgesPerProc, mapper.get_mpi_datatype(), 0, 0, MPI_COMM_WORLD, &status);
        }

        // calculate local MST with filter kruskal
        UnionFind uf(*n);
        WEdgeList mstList;
        mstList = filterKruskal::getMST(n, &localEdges, &uf);



        int j = 2;
        while (j <= ctx.size()) {
            int myMSTsize = (int) mstList.size();
            int otherMSTsize = 0;
            int i = 0;
            while (i < ctx.size()) {   // send local mst size to other processor
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
            while (i < ctx.size()) {   // send local mst to other processor
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

            //calculate new mst
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