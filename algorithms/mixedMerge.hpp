#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "mergeMST.hpp"
#include "boruvka_allreduce.hpp"
#include "filter_kruskal.hpp"


namespace mixed_merge {

    template<typename Timer>
    inline WEdgeList
    getMST(VId &vertexCount, WEdgeOriginList &e, size_t &localMSTcount,  std::vector<size_t> &numEdges,
           std::vector<size_t> &numVertices, Timer &timer, bool useKruskal = false, VId treeFactor = 2,
           size_t hashBorder = 1000) {

        timer.start("initVariables", 0);
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally
        hybridMST::mpi::TypeMapper<WEdgeOrigin> mapper;
        VId n = vertexCount;
        WEdgeOriginList mst;
        WEdgeOriginList incidentLocal;
        WEdgeOriginList incident;
        std::vector<VId> parent;
        std::vector<VId> vertices;
        size_t mstCount = localMSTcount;
        UnionFind uf(n);
        WEdgeOriginList edges;

        timer.stop("initVariables", 0);

        timer.start("mm-initial-localMST", 0);
        if (useKruskal) {
            edges = kruskal::getMST(e, uf);
        } else {
            edges = filterKruskal::getMST(n, e, uf);
        }
        if (mstCount > 0) {
            mstCount--;
        }

        timer.stop("mm-initial-localMST", 0);

        VId p = treeFactor;
        size_t iteration = 0;
        size_t limit = mergeMST::log_base(treeFactor, ctx.size());
        while (iteration < limit) {
            timer.start("b-iteration", iteration);
            size_t overlapCount = 0;
            boruvka_allreduce::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                                           overlapCount, timer, useKruskal, hashBorder, iteration);
            timer.stop("b-iteration", iteration);
            numVertices.push_back(n);
            numEdges.push_back(edges.size());

            timer.start("iteration", iteration);
            mergeMST::mergeStep(edges, p, uf, n, useKruskal, timer, treeFactor, iteration);
            timer.stop("iteration", iteration);
            numVertices.push_back(n);
            numEdges.push_back(edges.size());
            iteration++;
            p *= treeFactor;
        }

        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }

        return boruvka_allreduce::getOriginEdges(mst);
    }
}