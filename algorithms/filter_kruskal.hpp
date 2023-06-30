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

    template<typename Edge>
    inline VId pickPivot(std::span<Edge> &edges) {
        VId pivotWeight = 0;
        for (int i = 0; i < pivotEdgeCount; ++i) {
            VId w = rand() % (edges.size());
            w = edges[w].get_weight();
            pivotWeight += w;
        }
        return (VId) (pivotWeight / pivotEdgeCount);
    }

    inline size_t threshold(const VId &n) {
        return 3 * n;
    }

    template<typename Edge>
    inline void filter(std::span<Edge> &edges, UnionFind &uf) {
        std::remove_if(edges.begin(), edges.end(), [&](const auto &edge) {
            VId f1 = uf.find(edge.get_src());
            VId f2 = uf.find(edge.get_dst());
            return f1 == f2;
        });
    }


    template<typename Edge>
    inline void getMST(VId n, std::span<Edge> &edges, std::vector<Edge> &mst, UnionFind &uf) {
        if (edges.size() <= threshold(n)) {
            kruskal::getMST(edges, mst, uf);
            return;
        }
        VId pivotWeight = pickPivot(edges);

        auto middle = std::partition(edges.begin(), edges.end(), [&](const auto &edge) {
            return edge.get_weight() <= pivotWeight;
        });

        std::span<Edge> smaller(edges.begin(), middle);
        std::span<Edge> bigger(middle, edges.end());


        if (bigger.empty()) { //error prevention for infinite loop
            kruskal::getMST(smaller, mst, uf);
            return;
        }
        filterKruskal::getMST(n, smaller, mst, uf);
        filter(bigger, uf);
        filterKruskal::getMST(n, bigger, mst, uf);
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

        std::span<Edge> smaller(edges.begin(), middle);
        std::span<Edge> bigger(middle, edges.end());


        std::vector<Edge> mst;
        mst.reserve(n);

        if (bigger.empty()) { //error prevention for infinite loop
            kruskal::getMST(smaller, mst, uf);
            return mst;
        }
        filterKruskal::getMST(n, smaller, mst, uf);
        filter(bigger, uf);
        filterKruskal::getMST(n, bigger, mst, uf);

        return mst;
    }

}