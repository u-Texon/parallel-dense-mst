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
            } else if (in[i].get_weight() == inout[i].get_weight()) { //select edge with smaller indices
                if (in[i].get_src() + in[i].get_dst() < inout[i].get_src() + inout[i].get_dst()) {
                    inout[i] = in[i];
                }
            }
            //TODO: benötigt?
            /*
            if (inout[i].get_src() != i) { //swap src/dest for no ambiguity
                inout[i].set_dst(inout[i].get_src());
                inout[i].set_src(i);
            }*/
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

    void calcMinIncident(size_t n, WEdgeOriginList &incidentLocal, WEdgeOriginList &edges) {
        for (int i = 0; i < n; ++i) {
            incidentLocal[i] = WEdgeOrigin(i, i, -1); //initialize "empty" entries
        }
        for (auto &edge: edges) {
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




    void addMSTEdges(VId &n, WEdgeOriginList &mst, WEdgeOriginList &incident, WEdgeOriginList &edges) {
        //add edges to mst
        for (int i = 0; i < n; ++i) {
            if (incident[i].get_weight() != -1) {
                WEdgeOrigin &edge = incident[i];
                mst.push_back(edge);

                //mark edge as invalid, if it has already been added
                if (i == edge.get_src() && incident[edge.get_dst()] == edge) {
                    incident[edge.get_dst()].set_weight(-1);
                } else if (i == edge.get_dst() && incident[edge.get_src()] == edge) {
                    incident[edge.get_src()].set_weight(-1);
                }
            }
        }

        //continue with the edges that are not (yet) added to the mst
        WEdgeOriginList newEdges;
        for (auto &edge: edges) {
            VId u = edge.get_src();
            VId v = edge.get_dst();
            bool keep = true;
            if (edge == incident[u] || edge == incident[v]) {
                keep = false;
            }
            if (keep) {
                newEdges.push_back(edge);
            }
        }
        edges = newEdges;
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
                      WEdgeOriginList &edges, WEdgeOriginList &relabeledEdges) {
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
        for (auto &edge: edges) {
            VId s = parent[edge.get_src()];
            VId t = parent[edge.get_dst()];
            VId w = edge.get_weight();

            WEdgeOrigin r(vertices[s], edge.get_src_origin(), vertices[t], edge.get_dst_origin(), w);
            if (s != t) {
                relabeledEdges.push_back(r);
            }
        }
    }

    // TODO: remove or fix
    void removeParallelEdges(WEdgeOriginList &parallelEdges) {
        if (parallelEdges.empty()) {
            return;
        }

        std::sort(parallelEdges.begin(), parallelEdges.end(), kruskal::WeightOrder<WEdgeOrigin>{});
        WEdgeOriginList edges;

        //nach src, dst und dann weight sortieren

        //alternativ: bei vielen Duplicaten hashen:
        //wähle pivotgewicht sodass sample aus kanten ca. 5%, duplicate raus nehmen
        //src,dst paare in unordered set speichern
        //für alle größere elemente: schauen ob s,d drin ist  (aber nicht in hashmap)
        // danach base case

        for (auto &pE: parallelEdges) {
            bool contains = false;
            for (auto &e: edges) {
                if (e == pE) {
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
            //TODO: auswählöeen wie oft in jeder schleife
            edges = filterKruskal::getMST(n, edges, uf);

            //calculate edges incident to each vertex
            calcMinIncident(n, incidentLocal, edges);

            //perform all reduce to get the lightest edges for each vertex
            hybridMST::mpi::TypeMapper<WEdgeOrigin> mapper;
            MPI_Op reduce;
            MPI_Op_create((MPI_User_function *) minEdges, true, &reduce);
            MPI_Allreduce(incidentLocal.data(), incident.data(), (int) n, mapper.get_mpi_datatype(), reduce,
                          ctx.communicator());


            addMSTEdges(n, mst, incident, edges);

            fillParentArray(n, incident, parent);

            WEdgeOriginList relabeledEdges;
            relabelVandE(n, incident, parent, vertices, edges, relabeledEdges);
            edges = relabeledEdges;
        }


        WEdgeList returnEdges;
        for (auto &edge: mst) {
            returnEdges.push_back(edge.toWEdge());
        }
        return returnEdges;

    }


} //namespace