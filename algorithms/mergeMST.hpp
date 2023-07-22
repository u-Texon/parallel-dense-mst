#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"
#include "../include/util/NullTimer.hpp"

namespace mergeMST {


    size_t log_base(double base, double value) {
        return std::ceil(log(value) / log(base));
    }

    template<typename Edge, typename Timer>
    void
    mergeStep(std::vector<Edge> &edges, VId &p, UnionFind &uf, VId &n, bool &useKruskal, Timer &timer, VId &treeFactor,
              size_t iteration = 0) {
        hybridMST::mpi::MPIContext ctx;
        hybridMST::mpi::TypeMapper<Edge> mapper;
        hybridMST::mpi::TypeMapper<VId> intMapper;

        VId myMSTsize = edges.size();
        std::vector<VId> otherMSTsizes;
        otherMSTsizes.resize(treeFactor - 1);

        VId loopIndex = 0;
        timer.start("sendMST", iteration);
        while (loopIndex < ctx.size()) {   // send local mst size to other processor
            if (ctx.rank() == loopIndex) {
                for (int j = 0; j < treeFactor - 1; ++j) {
                    size_t senderID = loopIndex + (1 + j) * (p / treeFactor);
                    if (senderID < ctx.size()) {
                        MPI_Status status;
                        MPI_Recv(&otherMSTsizes[j], 1, intMapper.get_mpi_datatype(), (int) senderID, 0,
                                 ctx.communicator(), &status);


                    }
                }
            } else {
                for (int j = 0; j < treeFactor - 1; ++j) {
                    if (ctx.rank() == loopIndex + (1 + j) * (p / treeFactor)) {
                        MPI_Send(&myMSTsize, 1, intMapper.get_mpi_datatype(), (int) loopIndex, 0, ctx.communicator());
                        break;
                    }
                }
            }
            loopIndex += p;
        }


        std::vector<std::vector<Edge>> otherMSTs;
        otherMSTs.resize(treeFactor - 1);

        for (int j = 0; j < treeFactor - 1; ++j) {
            std::vector<Edge> mst;
            mst.resize(otherMSTsizes[j]);
            otherMSTs[j] = mst;
        }

        loopIndex = 0;
        while (loopIndex < ctx.size()) {   // send local mst to other processor
            if (ctx.rank() == loopIndex) {
                for (int j = 0; j < treeFactor - 1; ++j) {
                    size_t senderID = loopIndex + (1 + j) * (p / treeFactor);
                    if (senderID < ctx.size()) {
                        MPI_Status status;
                        MPI_Recv(otherMSTs[j].data(), otherMSTsizes[j], mapper.get_mpi_datatype(), (int) senderID, 0,
                                 ctx.communicator(), &status);
                    }
                }
            } else {
                for (int j = 0; j < treeFactor - 1; ++j) {
                    if (ctx.rank() == loopIndex + (1 + j) * (p / treeFactor)) {
                        MPI_Send(edges.data(), myMSTsize, mapper.get_mpi_datatype(), loopIndex, 0, ctx.communicator());
                        break;
                    }
                }
            }
            loopIndex += p;
        }

        timer.stop("sendMST", iteration);

        timer.start("localMST", iteration);
        //calculate new mst
        if (ctx.rank() % p == 0) {
            for (auto other: otherMSTs) {
                for (auto edge: other) {
                    edges.push_back(edge);
                }
            }

            uf.clear();
            if (useKruskal) {
                edges = kruskal::getMST(edges, uf);
            } else {
                edges = filterKruskal::getMST(n, edges, uf);
            }
        } else {
            edges.clear();
        }
        timer.stop("localMST", iteration);
    }

    template<typename Edge, typename Timer>
    inline std::vector<Edge>
    getMST(VId &n, std::vector<Edge> &edges, bool &useKruskal, Timer &timer, VId treeFactor = 2) {
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally


        // calculate local MST
        timer.start("initial-localMST", 0);
        UnionFind uf(n);
        std::vector<Edge> mstList;
        if (useKruskal) {
            mstList = kruskal::getMST(edges, uf);
        } else {
            mstList = filterKruskal::getMST(n, edges, uf);
        }
        timer.stop("initial-localMST", 0);


        VId p = treeFactor;
        size_t iteration = 0;
        size_t limit = log_base(treeFactor, ctx.size());
        while (iteration < limit) {
            timer.start("iteration", iteration);
            mergeStep(mstList, p, uf, n, useKruskal, timer, treeFactor, iteration);
            timer.stop("iteration", iteration);
            iteration++;
            p *= treeFactor;
        }
        return mstList;
    }

    template<typename Edge>
    inline std::vector<Edge>
    getBoxplot(VId &n, std::vector<Edge> &edges, std::vector<size_t> &numEdges, bool &useKruskal, VId treeFactor = 2) {
        hybridMST::mpi::MPIContext ctx;

        UnionFind uf(n);
        std::vector<Edge> mstList;
        if (useKruskal) {
            mstList = kruskal::getMST(edges, uf);
        } else {
            mstList = filterKruskal::getMST(n, edges, uf);
        }


        VId p = treeFactor;
        size_t iteration = 0;
        size_t limit = log_base(treeFactor, ctx.size());
        while (iteration < limit) {
            numEdges.push_back(edges.size());
            mergeStep(mstList, p, uf, n, useKruskal, NullTimer::getInstance(), treeFactor, iteration);
            iteration++;
            p *= treeFactor;
        }
        return mstList;
    }


} //namespace mergeMST