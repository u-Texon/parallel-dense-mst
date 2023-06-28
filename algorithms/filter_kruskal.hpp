#pragma once

#include "kruskal.hpp"
#include <span>



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
        return (VId) (pivotWeight / pivotEdgeCount);
    }

    inline size_t threshold(const VId &n) {
        return 300 * n;
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
    inline std::vector<Edge> getMST(VId n, std::vector<Edge> &edges, UnionFind &uf) {
        if (edges.size() <= threshold(n)) {
            return kruskal::getMST(edges, uf);
        }
        VId pivotWeight = pickPivot(edges);

        auto middle = std::partition(edges.begin(), edges.end(), [&](const auto &edge) {
            return edge.get_weight() <= pivotWeight;
        });


        //TODO: use span
        //std::vector myVec{1, 2, 3, 4, 5};
        //std::span mySpan1{myVec};


        std::vector<Edge> smaller(edges.begin(), middle);
        std::vector<Edge> bigger(middle, edges.end());


        if (bigger.empty()) { //error prevention for infinite loop
            return kruskal::getMST(smaller, uf);
        }


        //TODO: vector der größe n und dann immer einfügen wenn kante gefunden
        std::vector<Edge> mst;
        mst = filterKruskal::getMST(n, smaller, uf);
        filter(bigger, uf);
        std::vector<Edge> bigEdges = filterKruskal::getMST(n, bigger, uf);
        mst.insert(mst.end(), bigEdges.begin(), bigEdges.end());

        return mst;
    }

}