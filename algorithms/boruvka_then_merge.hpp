#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "mergeMST.hpp"
#include "dense_boruvka.hpp"
#include "filter_kruskal.hpp"



namespace boruvka_then_merge {




    inline WEdgeList getMST(VId &vertexCount, WEdgeOriginList &e) {
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
        VId c = 0;
        WEdgeOriginList edges = filterKruskal::getMST(n, e, uf, c);


        //TODO: correct border??
        VId border = (VId) (vertexCount / pow(2, log2(ctx.size())));

        while (n >= border) {
            dense_boruvka::boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, 1000);
        }

        edges = mergeMST::getMST(n, edges);


        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }

        return dense_boruvka::getOriginEdges(mst);
    }


}