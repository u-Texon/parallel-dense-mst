#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "mergeMST.hpp"
#include "boruvka_allreduce.hpp"
#include "filter_kruskal.hpp"


namespace mixed_merge {

    inline WEdgeList
    getMST(VId &vertexCount, WEdgeOriginList &e, size_t &localMSTcount, bool useKruskal = false, VId treeFactor = 2,
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


        VId p = treeFactor;
        size_t iteration = 0;
        size_t limit = mergeMST::log_base(treeFactor, ctx.size());
        while (iteration < limit) {
            boruvka_allreduce::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                                           NullTimer::getInstance(), useKruskal, hashBorder);
            mergeMST::mergeStep(edges, p, uf, n, useKruskal, NullTimer::getInstance(), treeFactor);
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


    inline WEdgeList
    getBoxplot(VId &vertexCount, WEdgeOriginList &e, size_t &localMSTcount, std::vector<size_t> &numEdges,
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
        numVertices.push_back(n);
        numEdges.push_back(edges.size());


        VId p = treeFactor;
        size_t iteration = 0;
        size_t limit = mergeMST::log_base(treeFactor, ctx.size());
        while (iteration < limit) {
            boruvka_allreduce::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                                           NullTimer::getInstance(), useKruskal, hashBorder);
            numVertices.push_back(n);
            numEdges.push_back(edges.size());
            mergeMST::mergeStep(edges, p, uf, n, useKruskal, NullTimer::getInstance(), treeFactor);
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