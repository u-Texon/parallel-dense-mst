#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"
#include "../include/dataStructures/Vertex.hpp"

namespace dense_boruvka {

    inline WEdgeList getMST(int *n, WEdgeList *edges) {
        if (*n == 1) {
            return {}; //base case
        }

        UnionFind uf(*n);
        WEdgeList newEdges = filterKruskal::getMST(n, edges, &uf);

        //calculate newEdges incident to each vertex
        WEdge incident[*n]; //keeps the lightest incident edges. e.g. incident[4] is the lightest edge incidcent to 4
        for (int i = 0; i < *n; ++i) {
            WEdge e(i, i, -1); //initialize "empty" entries
            incident[i] = e;
        }
        for (auto edge: newEdges) {
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


        WEdgeList mstEdges;
        for (auto edge: incident) {
            if (edge.get_weight() != -1) {
                mstEdges.push_back(edge);
            }
        }

        std::cout << "before trees worked" << std::endl;
        std::vector<Vertex> vertices;
        //pseudo trees
        for (int i = 0; i < *n; ++i) {
            VId id = i;
            VId parent;
            if (incident[i].get_weight() == -1) {
                parent = i;
            } else if (incident[i].get_src() == i) {
                parent = incident[i].get_dst();
            } else {
                parent = incident[i].get_src();
            }
            Vertex v(id, parent);
            vertices.push_back(v);
        }
        //rooted trees
        for (Vertex v: vertices) {
            Vertex parent = vertices.at(v.getParent());
            if (v.getID() == parent.getParent() && v.getID() < v.getParent()) {
                v.setParent(v.getID());
            }
        }

        //rooted stars
        for (Vertex v: vertices) {
            VId parent = v.getParent();
            while (parent != vertices.at(parent).getParent()) {
                v.setParent(vertices.at(parent).getParent());
                parent = vertices.at(parent).getParent();
            }
        }


        std::cout << "before relabel worked" << std::endl;
        //relable vertices
        int v = 1;
        for (int i = 1; i < *n; ++i) {
            if (vertices[i].getID() == vertices[i].getParent()) {
                v++;
            }
        }

        //relable edges
        WEdgeList e;
        for (auto edge: newEdges) {
            WEdge r;
            r.src = vertices[edge.get_src()].getParent();
            r.dst = vertices[edge.get_dst()].getParent();
            r.weight = edge.get_weight();
            e.push_back(r);
        }

        //remove parallel edges
        UnionFind uf2(v);
        e = filterKruskal::getMST(&v, &e, &uf2); //TODO: is this slow?

        //TODO: build edge-map for relabeling


        std::cout << "before recursion worked" << std::endl;
        WEdgeList otherMSTEdges = dense_boruvka::getMST(&v, &e);
        for (auto edge: otherMSTEdges) {
            mstEdges.push_back(edge);
        }
        return mstEdges;
    }


} //namespace