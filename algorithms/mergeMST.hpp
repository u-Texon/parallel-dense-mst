#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"

namespace mergeMST {




    template<typename Edge>
    void mergeStep(std::vector<Edge> &edges, VId &p, UnionFind &uf, VId &n) {
        hybridMST::mpi::MPIContext ctx;
        hybridMST::mpi::TypeMapper<WEdge> mapper;
        hybridMST::mpi::TypeMapper<VId> intMapper;

        VId myMSTsize = edges.size();
        VId otherMSTsize = 0;
        VId i = 0;
        while (i < ctx.size()) {   // send local mst size to other processor
            if (ctx.rank() == i) {
                MPI_Status status;
                MPI_Recv(&otherMSTsize, 1, intMapper.get_mpi_datatype(), (int) (i + p / 2), 0, ctx.communicator(), &status);
            } else if (ctx.rank() == i + p / 2) {
                MPI_Send(&myMSTsize, 1, intMapper.get_mpi_datatype(),(int) i, 0, ctx.communicator());
            }
            i += p;
        }
        std::vector<Edge> otherMST;
        otherMST.resize(otherMSTsize);


        i = 0;
        while (i < ctx.size()) {   // send local mst to other processor
            if (ctx.rank() == i) {
                MPI_Status status;
                MPI_Recv(otherMST.data(), otherMSTsize, mapper.get_mpi_datatype(), i + p / 2, 0, ctx.communicator(),
                         &status);
                break;
            } else if (ctx.rank() == i + p / 2) {
                MPI_Send(edges.data(), (int) edges.size(), mapper.get_mpi_datatype(), i, 0, ctx.communicator());
                break;
            }
            i += p;
        }

        //calculate new mst
        if (ctx.rank() % p == 0) {
            for (auto edge: otherMST) {
                edges.push_back(edge);
            }
            uf.clear();
            edges = filterKruskal::getMST(n, edges, uf);
        }
    }

    inline WEdgeList getMST(VId &n, WEdgeList &edges) {
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally


        // calculate local MST
        UnionFind uf(n);
        WEdgeList mstList;
        mstList = filterKruskal::getMST(n, edges, uf);


        VId p = 2;
        while (p <= ctx.size()) {
            mergeStep(mstList, p, uf, n);
            p *= 2;
        }
        return mstList;
    }



} //namespace mergeMST