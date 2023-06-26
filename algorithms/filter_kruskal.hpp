#pragma once

#include "kruskal.hpp"

#define maxLoops 10
#define pivotEdgeCount 10

namespace filterKruskal {

    template<typename Edge>
    inline VId pickPivot(std::vector<Edge> &edges) {
        VId pivotWeight = 0;
        for (int i = 0; i < pivotEdgeCount; ++i) {
            VId w = rand() % (edges.size());
            w = edges[w].get_weight();
            pivotWeight += w;
        }
        return (VId) (pivotWeight/pivotEdgeCount);
    }

    inline int threshold(const VId &n) {
        int t = 3 * n;
        return t;
    }

    template<typename Edge>
    inline void filter(std::vector<Edge> &edges, UnionFind &uf) {
        std::remove_if(edges.begin(), edges.end(), [&](const auto &edge) {
            VId f1 = uf.find(edge.get_src());
            VId f2 = uf.find(edge.get_dst());
            return f1 == f2;
        });
    }


    template<typename Edge>
    inline std::vector<Edge> getMST(VId n, std::vector<Edge> &edges, UnionFind &uf, VId &counter) {
        if (edges.size() <= threshold(n)) {
            return kruskal::getMST(edges, uf);
        }
        VId pivotWeight = pickPivot(edges);


        auto middle = std::partition(edges.begin(), edges.end(), [&](const auto &edge)
        {
            return edge.get_weight() <= pivotWeight;
        });
        std::vector<Edge> smaller(edges.begin(), middle);
        std::vector<Edge> bigger(middle, edges.end());



        if (bigger.empty()) { //error prevention for infinite loop
            counter++;
            if (counter >= maxLoops) {
                return kruskal::getMST(smaller, uf);
            }
        } else {
            counter = 0;
        }


        //TODO: vector der größe n und dann immer einfügen wenn kante gefunden
        std::vector<Edge> mst;
        mst = filterKruskal::getMST(n, smaller, uf, counter);
        filter(bigger, uf);
        std::vector<Edge> bigEdges = filterKruskal::getMST(n, bigger, uf, counter);
        mst.insert(mst.end(), bigEdges.begin(), bigEdges.end());

        return mst;
    }

}