#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"

namespace dense_boruvka {


    void reduceEdges(WEdgeOrigin *in, WEdgeOrigin *inout, const int *n, MPI_Datatype *datatype) {
        for (int i = 0; i < *n; i++) {
            if (in[i].get_weight() < inout[i].get_weight()) {
                inout[i] = in[i];
            }
        }
    }


    inline WEdgeList getMST(int &vertexCount, WEdgeOriginList &e) {



        hybridMST::mpi::MPIContext ctx;

        int n = vertexCount;
        WEdgeOriginList edges = e;
        WEdgeOriginList mst;
        WEdgeOriginList incidentLocal;
        WEdgeOriginList incident; //keeps the lightest incidentLocal edges. relabeledEdges.g. incidentLocal[4] is the lightest edge incidentLocal to vertex 4
        std::vector<VId> parent; //keeps the parent to the indexed vertex. relabeledEdges.g. parent[4] is the parent of vertex 4
        UnionFind uf(n);
        std::vector<VId> vertices;
        int iteration = 1; //TODO: remove
        while (n > 1) {
            //shrink arrays
            incidentLocal.resize(n);
            incident.resize(n);
            parent.resize(n);
            vertices.resize(n);
            uf.clear(n);

            //TODO: idee: processor mit kleinstem n broadcastet seine kanten


            //TODO: zuerst inzidente kanten schicken, für nebenläufige abarbeitung
            WEdgeOriginList newEdges = filterKruskal::getMST(n, edges, uf);

            //calculate edges incident to each vertex
            for (int i = 0; i < n; ++i) {
                incidentLocal[i] = WEdgeOrigin(i, i, -1); //initialize "empty" entries
            }
            for (auto &edge: newEdges) {
                VId u = edge.get_src();
                VId v = edge.get_dst();
                if (edge.get_weight() < incidentLocal[u].get_weight()) {
                    incidentLocal[u] = edge;
                }
                if (edge.get_weight() < incidentLocal[v].get_weight()) {
                    incidentLocal[v] = edge;
                }
            }



            //perform all reduce to get the lightest edges for each vertex
            hybridMST::mpi::TypeMapper<WEdgeOrigin> mapper;
            MPI_Op reduce;
            MPI_Op_create((MPI_User_function *) reduceEdges, true, &reduce);
            MPI_Reduce(incidentLocal.data(), incident.data(), n, mapper.get_mpi_datatype(), reduce, 0,
                          ctx.communicator());
            MPI_Bcast(incident.data(), n, mapper.get_mpi_datatype(), 0, ctx.communicator());
            //TODO: with allreduce instead of broadcast errors occur because edge-src/dst can be swopped





            //add edges to mst
            WEdgeOriginList mstEdges;
            for (int i = 0; i < n; ++i) {
                WEdgeOrigin edge = incident[i];
                if (edge.get_weight() != -1) {
                    //don't add the same edge twice
                    bool contains = false;
                    for (auto &mstEdge: mstEdges) {
                        if (mstEdge.get_src() == edge.get_src() && mstEdge.get_dst() == edge.get_dst()) {
                            contains = true;
                            break;
                        }
                        if (mstEdge.get_dst() == edge.get_src() && mstEdge.get_src() == edge.get_dst()) {
                            contains = true;
                            break;
                        }
                    }
                    if (!contains) {
                        mstEdges.push_back(edge);
                        mst.push_back(edge);
                    }
                }
            }

            //continue with the edges that are not (yet) added to the mst
            WEdgeOriginList newEdges2;
            for (auto &edge: newEdges) {
                VId u = edge.get_src();
                VId v = edge.get_dst();
                bool keep = true;
                if (edge == incident[u] || edge == incident[v]) {
                    keep = false;
                }
                if (keep) {
                    newEdges2.push_back(edge);
                }
            }
            newEdges = newEdges2;

            //pseudo trees
            for (int i = 0; i < n; ++i) {
                VId p;
                if (incident[i].get_weight() == -1) {
                    p = i;
                } else if (incident[i].get_src() == i) {
                    p = incident[i].get_dst();
                } else {
                    p = incident[i].get_src();
                }
                parent[i] = p;
            }

            //rooted trees
            for (int i = 0; i < n; ++i) {
                if (parent[parent[i]] == i) {
                    parent[i] = i;
                }
            }

            //rooted stars
            for (int i = 0; i < n; ++i) {
                while (parent[parent[i]] != parent[i]) {
                    parent[i] = parent[parent[i]];
                }
            }

            //relable vertices
            int v = 0;
            for (int i = 0; i < n; ++i) {
                if (parent[i] == i && incident[i].get_weight() != -1) {
                    vertices[i] = v;
                    v++;
                }
            }


            //relable edges
            WEdgeOriginList relabeledEdges;
            for (auto &edge: newEdges) {
                VId s = parent[edge.get_src()];
                VId t = parent[edge.get_dst()];
                VId w = edge.get_weight();

                WEdgeOrigin r(vertices[s], edge.get_src_origin(), vertices[t], edge.get_dst_origin(), w);
                if (s != t) {
                    relabeledEdges.push_back(r);
                }
            }


            //remove parallel edges
            uf.clear();
            relabeledEdges = filterKruskal::getMST(n, relabeledEdges, uf);
            //TODO: ausprobieren was schneller ist. z.b sortieren (ohne union find)

            n = v;
            edges = relabeledEdges;
            iteration++;
        }


        WEdgeList returnEdges;
        for (auto &edge: mst) {
            returnEdges.push_back(edge.toWEdge());
        }
        return returnEdges;

    }


} //namespace