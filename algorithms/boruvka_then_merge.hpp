#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "mergeMST.hpp"
#include "boruvka_allreduce.hpp"
#include "filter_kruskal.hpp"

namespace boruvka_then_merge {

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


        VId num = mergeMST::log_base(treeFactor, ctx.size());
        VId border = (VId) (vertexCount / pow(2, num));

        while (n >= border) {
            boruvka_allreduce::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                                           NullTimer::getInstance(),
                                           useKruskal, hashBorder);
        }

        edges = mergeMST::getMST(n, edges, useKruskal, NullTimer::getInstance(), treeFactor);


        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }

        return boruvka_allreduce::getOriginEdges(mst);
    }

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

        numVertices.push_back(n);
        numEdges.push_back(e.size());

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

        while (n >= border) {
            boruvka_allreduce::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                                           NullTimer::getInstance(), useKruskal, hashBorder);
            numVertices.push_back(n);
            numEdges.push_back(edges.size());
        }
        edges = mergeMST::getBoxplot(n, edges, numEdges, useKruskal, treeFactor);


        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }

    }
}