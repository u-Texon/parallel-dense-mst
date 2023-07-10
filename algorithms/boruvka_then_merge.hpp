#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "mergeMST.hpp"
#include "dense_boruvka.hpp"
#include "filter_kruskal.hpp"

namespace boruvka_then_merge {

    inline WEdgeList getMST(VId &vertexCount, WEdgeOriginList &e, bool useKruskal, VId treeFactor = 2) {
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


        VId num = mergeMST::log_base(treeFactor, ctx.size());
        VId border = (VId) (vertexCount / pow(2, num));

        while (n >= border) {
            dense_boruvka::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, 1000);
        }

        edges = mergeMST::getMST(n, edges, useKruskal, treeFactor);


        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }

        return dense_boruvka::getOriginEdges(mst);
    }
}