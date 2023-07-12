#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "mergeMST.hpp"
#include "dense_boruvka.hpp"
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


        //compute local mst
        UnionFind uf(n);
        WEdgeOriginList edges;
        if (useKruskal) {
            edges = kruskal::getMST(e, uf);
        } else {
            edges = filterKruskal::getMST(n, e, uf);
        }
        localMSTcount --;

        VId p = 2;

        hybridMST::Timer t;

        while (p <= ctx.size()) {
            if (ctx.rank() == 0) {
                std::cout << "edges: " << e.size() << std::endl;
                std::cout<< "mst: " << mst.size() << std::endl;
            }
            dense_boruvka::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, localMSTcount, t,
                                       useKruskal, hashBorder);
            mergeMST::mergeStep(edges, p, uf, n, useKruskal, treeFactor);
            p *= 2;
        }

        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }

        return dense_boruvka::getOriginEdges(mst);
    }


}