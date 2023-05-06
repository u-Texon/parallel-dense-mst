#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"


namespace dense_boruvka {

    inline WEdgeList getMST(int *n, WEdgeList *edges) {

        //maybe distribute edges, or not
        //...


        UnionFind uf(*n);
        WEdgeList mst = filterKruskal::getMST(n, edges, &uf);

        //calculate mst incident to each vertex
        WEdge incident[*n]; //keeps the lightest incident edges. e.g. incident[4] is the lightest edge incidcent to 4
        for (int i = 0; i < *n; ++i) {
            WEdge e(i, i, -1); //fill "empty" entries
            incident[i] = e;
        }

        for (auto edge: mst) {
            VId u = edge.get_src();
            VId v = edge.get_dst();

            if (incident[u].get_weight() > edge.get_weight()) {
                incident[u] = edge;
            }
            if (incident[v].get_weight() > edge.get_weight()) {
                incident[v] = edge;
            }
        }


        //perform all reduce to get the lightest edges for each vertex
        hybridMST::mpi::MPIContext ctx;
        hybridMST::mpi::TypeMapper<WEdge> mapper;

        if (ctx.rank() == 0) {
            WEdge other[*n];
            MPI_Status status;
            for (int i = 1; i < ctx.size(); ++i) {
                MPI_Recv(other, *n, mapper.get_mpi_datatype(), i, 0, ctx.communicator(), &status);
                for (int j = 0; j < *n; ++j) {
                    if (other[j].get_weight() < incident[j].get_weight()) {
                        incident[j] = other[j];
                    }
                }
            }
        } else {
            MPI_Send(incident, *n, mapper.get_mpi_datatype(), 0, 0, ctx.communicator());
        }

        MPI_Bcast(incident, *n, mapper.get_mpi_datatype(), 0, ctx.communicator());

        if (ctx.rank() == 0) {
            for (int i = 0; i < *n; ++i) {
                std::cout << "(" << incident[i].get_src() << "," << incident[i].get_dst() << ","
                          << incident[i].get_weight() << ")";
            }
            std::cout << std::endl;
        }


        return mst;
    }


} //namespace