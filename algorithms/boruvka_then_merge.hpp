#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "mergeMST.hpp"
#include "boruvka_allreduce.hpp"
#include "filter_kruskal.hpp"

namespace boruvka_then_merge {

    template<typename Timer>
    inline WEdgeList
    getMST(VId &vertexCount, WEdgeOriginList &e, size_t &localMSTcount, Timer &timer, bool useKruskal = false,
           VId treeFactor = 2, size_t hashBorder = 1000) {

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

        timer.start("b-initial-localMST", 0);
        if (useKruskal) {
            edges = kruskal::getMST(e, uf);
        } else {
            edges = filterKruskal::getMST(n, e, uf);
        }
        mstCount--;
        timer.stop("b-initial-localMST", 0);

        VId num = mergeMST::log_base(treeFactor, ctx.size());
        VId border = (VId) (vertexCount / pow(2, num));

        size_t iteration = 0;
        while (n >= border) {
            timer.start("b-iteration", iteration);
            boruvka_allreduce::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                                           timer, useKruskal, hashBorder, iteration);
            timer.stop("b-iteration", iteration);
            iteration++;
        }

        edges = mergeMST::getMST(n, edges, useKruskal, timer, treeFactor);


        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }

        return boruvka_allreduce::getOriginEdges(mst);
    }

    /**
     * only used for creating box plots
     */
    void getBoxplot(VId &vertexCount, WEdgeOriginList &e, size_t &localMSTcount, std::vector<size_t> &numEdges,
                    std::vector<size_t> &numVertices, bool useKruskal = false, VId treeFactor = 2,
                    size_t hashBorder = 1000) {
        hybridMST::mpi::MPIContext ctx; // calls MPI_Init internally
        hybridMST::mpi::TypeMapper<WEdgeOrigin> mapper;
        VId n = vertexCount;
        WEdgeOriginList mst;
        WEdgeOriginList incidentLocal;
        WEdgeOriginList incident;
        std::vector<VId> parent;
        std::vector<VId> vertices;


        size_t mstCount = localMSTcount;

        //compute local mst
        UnionFind uf(n);
        WEdgeOriginList edges;
        if (useKruskal) {
            edges = kruskal::getMST(e, uf);
        } else {
            edges = filterKruskal::getMST(n, e, uf);
        }
        mstCount--;


        VId num = mergeMST::log_base(treeFactor, ctx.size());
        VId border = (VId) (vertexCount / pow(2, num));


        numVertices.push_back(n);
        numEdges.push_back(edges.size());
        NullTimer nullTimer = NullTimer();


        while (n >= border) {
            boruvka_allreduce::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                                           nullTimer, useKruskal, hashBorder);
            numVertices.push_back(n);
            numEdges.push_back(edges.size());
        }
        edges = mergeMST::getBoxplot(n, edges, numEdges, numVertices, useKruskal, treeFactor);


        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }

    }
}