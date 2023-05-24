#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "mergeMST.hpp"
#include "dense_boruvka.hpp"
#include "filter_kruskal.hpp"


namespace mixed_merge {

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
        WEdgeOriginList edges = filterKruskal::getMST(n, e, uf);


        VId p = 2;
        while (p <= ctx.size()) {
            dense_boruvka::boruvkaSTep(n, incidentLocal, incident, vertices, parent, uf, edges, mst);
            mergeMST::mergeStep(edges, p, uf, n);

            p *= 2;
        }



        //base case

        if (ctx.rank() == 0) {
            for (auto edge: edges) {
                mst.push_back(edge);
            }
        }





        return dense_boruvka::getOriginEdges(mst);


    }


}