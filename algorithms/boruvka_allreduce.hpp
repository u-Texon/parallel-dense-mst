#pragma once

#include "../include/mpi/context.hpp"
#include "../include/definitions.hpp"
#include "../include/mpi/type_handling.hpp"
#include "filter_kruskal.hpp"
#include "kruskal.hpp"
#include "ips4o.hpp"
#include "../include/util/NullTimer.hpp"
#include <thread>

namespace boruvka_allreduce {


    /**
     * mpi allreduce function
     */
    void minEdges(WEdgeOrigin *in, WEdgeOrigin *inout, const int *n, [[maybe_unused]] MPI_Datatype *datatype) {
        for (int i = 0; i < *n; i++) {
            if (in[i].get_weight() < inout[i].get_weight()) { //save edge with min weight
                inout[i] = in[i];
            } else if (in[i].get_weight() == inout[i].get_weight()) { //select edge with smaller indices
                if (in[i].get_src() + in[i].get_dst() < inout[i].get_src() + inout[i].get_dst()) {
                    inout[i] = in[i];
                }
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
        edges.erase(std::remove_if(edges.begin(), edges.end(), [&](const auto &edge) {
            return edge == incident[edge.get_src()] || edge == incident[edge.get_dst()];
        }), edges.end());

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

    void relabel_V_E(VId &n, WEdgeOriginList &incident, std::vector<VId> &parent, std::vector<VId> &vertices,
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


    void removeParallelEdges(WEdgeOriginList &parallelEdges) {
        if (parallelEdges.empty()) {
            return;
        }
        ips4o::sort(parallelEdges.begin(), parallelEdges.end(), SrcDstWeightOrder<WEdgeOrigin>{});

        WEdgeOriginList edges;
        VId s = -1;
        VId d = -1;
        for (auto &e: parallelEdges) {
            if (e.get_src() != s) {
                edges.push_back(e);
                s = e.get_src();
                d = e.get_dst();
            } else if (e.get_dst() != d) {
                edges.push_back(e);
                d = e.get_dst();
            }
        }
        parallelEdges = edges;
    }


    struct pair_hash {
        inline std::size_t operator()(const std::pair<int, int> &v) const {
            return v.first * 431 + v.second;
        }
    };

    /**
     *  bei vielen Duplicaten hashen:
     *  wähle pivotgewicht sodass sample aus kanten ca. 5%, duplicate raus nehmen
     *  src,dst paare in unordered set speichern
     *  für alle größere elemente: schauen ob s,t drin ist  (aber nicht in hashmap)
     *  danach base case
     *
     * @param parallelEdges
     */
    void removeParallelEdgesHashing(WEdgeOriginList &parallelEdges) {
        WEdgeOriginList pivotEdges;

        //estimate pivot weight that is roughly the 5% lowest of all edges
        for (int i = 0; i < 100; ++i) {
            pivotEdges.push_back(parallelEdges[i]);
        }
        ips4o::sort(pivotEdges.begin(), pivotEdges.end(), WeightOrder<WEdgeOrigin>{});
        VId pivot = pivotEdges[4].get_weight();

        //separate light edges from heavy edges
        std::unordered_set<std::pair<VId, VId>, pair_hash> set;
        WEdgeOriginList smaller, bigger;
        for (auto &edge: parallelEdges) {
            if (edge.get_weight() <= pivot) {
                smaller.push_back(edge);
            } else {
                bigger.push_back(edge);
            }
        }

        //safe the low weight edges into a hashset
        WEdgeOriginList edges;
        for (auto &edge: smaller) {
            VId s = edge.get_src();
            VId t = edge.get_dst();

            if (edge.get_src() > edge.get_dst()) {
                s = edge.get_dst();
                t = edge.get_src();
            }

            if (set.find({s, t}) == set.end()) {
                set.insert({s, t});
                edges.push_back(edge);
            }
        }

        //remove heavy edges
        for (auto &edge: bigger) {
            VId s = edge.get_src();
            VId t = edge.get_dst();
            if (edge.get_src() > edge.get_dst()) {
                s = edge.get_dst();
                t = edge.get_src();
            }
            if (set.find({s, t}) != set.end()) {
                edges.push_back(edge);
            }
        }

        //finish with base case
        return removeParallelEdges(edges);
    }


    void allReduce(VId n, WEdgeOriginList &incidentLocal, WEdgeOriginList &incident) {
        hybridMST::mpi::MPIContext ctx;
        hybridMST::mpi::TypeMapper<WEdgeOrigin> mapper;
        MPI_Op reduce;
        MPI_Op_create((MPI_User_function *) minEdges, true, &reduce);
        MPI_Allreduce(incidentLocal.data(), incident.data(), (int) n, mapper.get_mpi_datatype(), reduce,
                      ctx.communicator());
    }


    void localMST(VId n, WEdgeOriginList &edges, bool useKruskal, size_t &localMSTcount, UnionFind &uf) {
        if (localMSTcount > 0) {
            if (useKruskal) {
                edges = kruskal::getMST(edges, uf);
            } else {
                edges = filterKruskal::getMST(n, edges, uf);
            }
            localMSTcount--;
        }
    }

    template<typename Timer>
    void
    boruvkaStepThread(VId &n, WEdgeOriginList &incidentLocal, WEdgeOriginList &incident, std::vector<VId> &vertices,
                      std::vector<VId> &parent, UnionFind &uf, WEdgeOriginList &edges, WEdgeOriginList &mst,
                      size_t &localMSTcount, Timer &timer, bool useKruskal = false, size_t hashBorder = 1000,
                      size_t iteration = 0) {

        shrink(n, incidentLocal, incident, vertices, parent, uf);


        calcMinIncident(n, incidentLocal, edges);


        std::thread threadAllReduce(allReduce, n, std::ref(incidentLocal), std::ref(incident));
        std::thread threadLocalMST(localMST, n, std::ref(edges), useKruskal, std::ref(localMSTcount), std::ref(uf));

        threadAllReduce.join();
        threadLocalMST.join();

        addMSTEdges(n, mst, incident, edges);
        fillParentArray(n, incident, parent);

        WEdgeOriginList relabeledEdges;
        relabel_V_E(n, incident, parent, vertices, edges, relabeledEdges);


        if (relabeledEdges.size() > hashBorder) {
            removeParallelEdgesHashing(relabeledEdges);
        } else {
            removeParallelEdges(relabeledEdges);
        }

        edges = relabeledEdges;
    }

    template<typename Timer>
    void boruvkaStep(VId &n, WEdgeOriginList &incidentLocal, WEdgeOriginList &incident, std::vector<VId> &vertices,
                     std::vector<VId> &parent, UnionFind &uf, WEdgeOriginList &edges, WEdgeOriginList &mst,
                     size_t &localMSTcount, Timer &timer, bool useKruskal = false,
                     size_t hashBorder = 1000,
                     size_t iteration = 0) {

        timer.start("shrink", iteration);
        shrink(n, incidentLocal, incident, vertices, parent, uf);
        timer.stop("shrink", iteration);

        if (localMSTcount > 0) {
            timer.start("calcLocalMST", iteration);
            if (useKruskal) {
                edges = kruskal::getMST(edges, uf);
            } else {
                edges = filterKruskal::getMST(n, edges, uf);
            }
            localMSTcount--;
            timer.stop("calcLocalMST", iteration);
        }


        timer.start("calc-incident", iteration);
        calcMinIncident(n, incidentLocal, edges);
        timer.stop("calc-incident", iteration);


        timer.start("allreduce", iteration);
        allReduce(n, incidentLocal, incident);
        timer.stop("allreduce", iteration);

        hybridMST::mpi::MPIContext ctx;
        timer.start("parentArray", iteration);
        if (ctx.rank() == 0) {
            addMSTEdges(n, mst, incident, edges);
        }
        fillParentArray(n, incident, parent);
        timer.stop("parentArray", iteration);

        timer.start("relabel", iteration);
        WEdgeOriginList relabeledEdges;
        relabel_V_E(n, incident, parent, vertices, edges, relabeledEdges);
        timer.stop("relabel", iteration);


        timer.start("removeParallelEdges", iteration);
        if (relabeledEdges.size() > hashBorder) {
            //TODO: needs to be tested
            removeParallelEdgesHashing(relabeledEdges);
        } else {
            removeParallelEdges(relabeledEdges);
        }
        timer.stop("removeParallelEdges", iteration);

        edges = relabeledEdges;


    }

    WEdgeList getOriginEdges(WEdgeOriginList &mst) {
        WEdgeList returnEdges;
        for (auto &edge: mst) {
            returnEdges.push_back(edge.toWEdge());
        }
        return returnEdges;
    }

    template<typename Timer>
    inline WEdgeList getMST(VId &vertexCount, WEdgeOriginList &e, size_t &localMSTcount,
                            Timer &timer, bool useThreads = false, bool useKruskal = false,
                            size_t hashBorder = 1000) {

        timer.start("initVariables", 0);
        VId n = vertexCount;
        WEdgeOriginList edges = e;
        WEdgeOriginList mst;
        WEdgeOriginList incidentLocal;
        WEdgeOriginList incident; //keeps the lightest incidentLocal edges. relabeledEdges.g. incidentLocal[4] is the lightest edge incidentLocal to vertex 4
        std::vector<VId> parent; //keeps the parent to the indexed vertex. relabeledEdges.g. parent[4] is the parent of vertex 4
        UnionFind uf(n);
        std::vector<VId> vertices;
        size_t mstCount = localMSTcount;
        size_t iteration = 1;
        timer.stop("initVariables", 0);

        while (n > 1) {
            timer.start("iteration", iteration);
            if (useThreads) {
                boruvkaStepThread(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount, timer,
                                  useKruskal, hashBorder, iteration);
            } else {
                boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount, timer, useKruskal,
                            hashBorder, iteration);
            }
            timer.stop("iteration", iteration);
            iteration++;
        }

        return getOriginEdges(mst);

    }

    inline WEdgeList
    getBoxplot(VId &vertexCount, WEdgeOriginList &e, size_t &localMSTcount, std::vector<size_t> &numEdges,
               std::vector<size_t> &numVertices, bool useThreads = false, bool useKruskal = false,
               size_t hashBorder = 1000) {

        VId n = vertexCount;
        WEdgeOriginList edges = e;
        WEdgeOriginList mst;
        WEdgeOriginList incidentLocal;
        WEdgeOriginList incident; //keeps the lightest incidentLocal edges. relabeledEdges.g. incidentLocal[4] is the lightest edge incidentLocal to vertex 4
        std::vector<VId> parent; //keeps the parent to the indexed vertex. relabeledEdges.g. parent[4] is the parent of vertex 4
        UnionFind uf(n);
        std::vector<VId> vertices;
        size_t mstCount = localMSTcount;
        size_t iteration = 1;

        NullTimer nullTimer = NullTimer();
        while (n > 1) {
            if (useThreads) {
                boruvkaStepThread(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                                  nullTimer,
                                  useKruskal, hashBorder, iteration);
            } else {
                boruvkaStep(n, incidentLocal, incident, vertices, parent, uf, edges, mst, mstCount,
                            nullTimer, useKruskal,
                            hashBorder, iteration);
            }
            iteration++;
            numEdges.push_back(edges.size());
            numVertices.push_back(n);
        }


        return getOriginEdges(mst);

    }


} //namespace