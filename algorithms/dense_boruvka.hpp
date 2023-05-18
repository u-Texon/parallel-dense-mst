#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"
#include "kruskal.hpp"

namespace dense_boruvka {


    /*
     * allreduce function
     */
    void minEdges(WEdgeOrigin *in, WEdgeOrigin *inout, const int *n, MPI_Datatype *datatype) {
        for (int i = 0; i < *n; i++) {
            if (in[i].get_weight() < inout[i].get_weight()) { //save edge with min weight
                inout[i] = in[i];
            } else if (in[i].get_weight() == inout[i].get_weight()) { //select edge with smaller destination
                if (in[i].get_src() + in[i].get_dst() < inout[i].get_src() + inout[i].get_dst()) {
                    inout[i] = in[i];
                }
            }
            if (inout[i].get_src() != i) { //swap src/dest for no ambiguity
                inout[i].set_dst(inout[i].get_src());
                inout[i].set_src(i);
            }
        }
    }


    void
    shrink(size_t n, WEdgeOriginList &a, WEdgeOriginList &b, std::vector<VId> &c, std::vector<VId> &d, UnionFind &uf) {
        a.resize(n);
        b.resize(n);
        c.resize(n);
        d.resize(n);
        uf.clear(n);
    }

    void calcMinIncident(size_t n, WEdgeOriginList &incidentLocal, WEdgeOriginList &newEdges) {
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
    }


    void removeParallelEdges(WEdgeOriginList &parallelEdges) {
        if (parallelEdges.empty()) {
            return;
        }

        std::sort(parallelEdges.begin(), parallelEdges.end(), kruskal::WeightOrder<WEdgeOrigin>{});
        WEdgeOriginList edges;
        for (auto &pE: parallelEdges) {
            bool contains = false;
            for (auto &e: edges) {
                if (e.get_src() == pE.get_src() && e.get_dst() == pE.get_dst()
                    || e.get_dst() == pE.get_src() && e.get_src() == pE.get_dst()) {
                    contains = true;
                    break;
                }
            }
            if (!contains) {
                edges.push_back(pE);
            }
        }
        parallelEdges = edges;
    }


    void addMSTEdges(VId &n, WEdgeOriginList &mst, WEdgeOriginList &incident, WEdgeOriginList &newEdges) {
        //add edges to mst
        for (int i = 0; i < n; ++i) {
            if (incident[i].get_weight() != -1) {
                WEdgeOrigin edge = incident[i];
                mst.push_back(edge);
                //mark edge as invalid, if it has already been added
                if (i == incident[edge.get_dst()].get_dst()) {
                    incident[edge.get_dst()].set_weight(-1);
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
    }


    void fillParentArray(VId &n, WEdgeOriginList &incident, std::vector<VId> &parent) {
        //pseudo trees
        for (int i = 0; i < n; ++i) {
            VId p;
            if (incident[i].get_src() == incident[i].get_dst()) {
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
    }

    void relabelVandE(VId &n, WEdgeOriginList &incident, std::vector<VId> &parent, std::vector<VId> &vertices,
                      WEdgeOriginList &newEdges, WEdgeOriginList &relabeledEdges) {
        //relable vertices
        int v = 0;
        for (int i = 0; i < n; ++i) {
            if (parent[i] == i && incident[i].get_src() != incident[i].get_dst()) {
                vertices[i] = v;
                v++;
            }
        }

        n = v;


        //relable edges
        for (auto &edge: newEdges) {
            VId s = parent[edge.get_src()];
            VId t = parent[edge.get_dst()];
            VId w = edge.get_weight();

            WEdgeOrigin r(vertices[s], edge.get_src_origin(), vertices[t], edge.get_dst_origin(), w);
            if (s != t) {
                relabeledEdges.push_back(r);
            }
        }
    }


    inline WEdgeList getMST(int &vertexCount, WEdgeOriginList &e) {
        hybridMST::mpi::MPIContext ctx;
        VId n = vertexCount;
        WEdgeOriginList edges = e;
        WEdgeOriginList mst;
        WEdgeOriginList incidentLocal;
        WEdgeOriginList incident; //keeps the lightest incidentLocal edges. relabeledEdges.g. incidentLocal[4] is the lightest edge incidentLocal to vertex 4
        std::vector<VId> parent; //keeps the parent to the indexed vertex. relabeledEdges.g. parent[4] is the parent of vertex 4
        UnionFind uf(n);
        std::vector<VId> vertices;


        while (n > 1) {
            //shrink arrays
            shrink(n, incidentLocal, incident, vertices, parent, uf);

            //TODO: zuerst inzidente kanten schicken, für nebenläufige abarbeitung
            WEdgeOriginList newEdges = filterKruskal::getMST(n, edges, uf);

            //calculate edges incident to each vertex
            calcMinIncident(n, incidentLocal, newEdges);

            //perform all reduce to get the lightest edges for each vertex
            hybridMST::mpi::TypeMapper<WEdgeOrigin> mapper;
            MPI_Op reduce;
            MPI_Op_create((MPI_User_function *) minEdges, true, &reduce);
            MPI_Allreduce(incidentLocal.data(), incident.data(), (int) n, mapper.get_mpi_datatype(), reduce,
                          ctx.communicator());


            addMSTEdges(n, mst, incident, newEdges);

            fillParentArray(n, incident, parent);

            WEdgeOriginList relabeledEdges;
            relabelVandE(n, incident, parent, vertices, newEdges, relabeledEdges);

            //removeParallelEdges(relabeledEdges);
            uf.clear();
            relabeledEdges = filterKruskal::getMST(n, relabeledEdges, uf);
            //TODO: ausprobieren was schneller ist. z.b sortieren (ohne union find)

            edges = relabeledEdges;
        }


        WEdgeList returnEdges;
        for (auto &edge: mst) {
            returnEdges.push_back(edge.toWEdge());
        }
        return returnEdges;

    }


} //namespace