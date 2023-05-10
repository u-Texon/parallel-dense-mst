#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"
#include "../include/dataStructures/Vertex.hpp"

namespace dense_boruvka {

    inline WEdgeList getMST(int *n, WEdgeList *edges) {
        hybridMST::mpi::MPIContext ctx;
        if (*n == 1) {
            return {}; //base case
        }

        UnionFind uf(*n);
        WEdgeList newEdges = filterKruskal::getMST(n, edges, &uf);

        //calculate newEdges incident to each vertex
        WEdge incident[*n]; //keeps the lightest incident edges. edgeList.g. incident[4] is the lightest edge incident to 4
        for (int i = 0; i < *n; ++i) {
            incident[i] = WEdge(i, i, -1); //initialize "empty" entries
        }
        WEdgeList newEdges2;
        for (auto edge: newEdges) {
            VId u = edge.get_src();
            VId v = edge.get_dst();
            bool keep = true;
            if (incident[u].get_weight() > edge.get_weight()) {
                incident[u] = edge;
                keep = false;
            }
            if (incident[v].get_weight() > edge.get_weight()) {
                incident[v] = edge;
                keep = false;
            }
            if (keep) {
                newEdges2.push_back(edge);
            }
        }
        newEdges = newEdges2;




        //perform all reduce to get the lightest edges for each vertex
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

        UnionFind uf3(*n);
        mstEdges = filterKruskal::getMST(n, &mstEdges, &uf3); //remove duplicate edges

        //pseudo trees
        int x = 0;
        std::vector<Vertex> vertices;
        for (int i = 0; i < *n; ++i) {
            VId id = i;
            VId parent;
            if (incident[i].get_weight() == -1) {
                parent = i;
                x++;
            } else if (incident[i].get_src() == i) {
                parent = incident[i].get_dst();
            } else {
                parent = incident[i].get_src();
            }
            Vertex v(id, parent);
            vertices.push_back(v);
        }

        if (x == *n) {
            return mstEdges;
        }


        //rooted trees
        for (Vertex v: vertices) {
            Vertex parent = vertices.at(v.getParent());
            if (v.getID() == parent.getParent() && v.getID() > parent.getID()) {
                vertices.at(v.getParent()).setParent(parent.getID());
            }
        }

        //rooted stars
        std::vector<Vertex> vertices2; //TODO: why is this necessary ????
        for (Vertex v: vertices) {
            while (v.getParent() != vertices.at(v.getParent()).getParent()) {
                VId old = v.getParent();
                v.setParent(vertices.at(v.getParent()).getParent());
            }
            vertices2.push_back(v);
        }
        vertices = vertices2;

        //relable vertices
        int v = 0;
        for (int i = 0; i < *n; ++i) {
            if (vertices[i].getID() == vertices[i].getParent()) {
                v++;
            }
        }
        std::map<VId , VId> map;
        VId i = 0;
        for (Vertex vertex: vertices) {
            if (vertex.getID() == vertex.getParent()) {
                if (map.find(vertex.getID()) == map.end()) {
                    map.insert({ vertex.getID(), i});
                    i++;
                }
            }
        }



        //relable edges
        WEdgeList edgeList;
        for (auto edge: newEdges) {
            VId s = vertices[edge.get_src()].getParent();
            VId t = vertices[edge.get_dst()].getParent();
            VId w = edge.get_weight();

            WEdge r(map.find(s)->second, map.find(t)->second, w);
            if (s != t) {
                edgeList.push_back(r);
            }
        }



        //remove parallel edges
        UnionFind uf2(v);
        edgeList = filterKruskal::getMST(n, &edgeList, &uf2); //TODO: is this slow?



        //TODO: build edge-map for relabeling

        WEdgeList otherMSTEdges = dense_boruvka::getMST(&v, &edgeList);
        for (auto edge: otherMSTEdges) {
            mstEdges.push_back(edge);
        }

        return mstEdges;
    }


} //namespace