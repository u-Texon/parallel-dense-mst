#pragma once

#include "kruskal.hpp"
#include <span>
#include <random>
#include <chrono>




namespace filterKruskal {

    template<typename Edge>
    inline VId pickPivot(std::span<Edge> &edges) {
        std::size_t pivotEdgeCount = 50;
        pivotEdgeCount = std::min(edges.size(), pivotEdgeCount);
        std::mt19937 gen;
        std::uniform_int_distribution<std::size_t> distrib(0, edges.size() - 1);
        VId pivotWeight = 0;
        std::vector<VId> weights(pivotEdgeCount);
        for (int i = 0; i < pivotEdgeCount; ++i) {
            weights[i] = edges[distrib(gen)].get_weight();
        } //TODO: sample holen, sortieren und mdeian zurückgeben
        std::sort(weights.begin(), weights.end());
        return weights[pivotEdgeCount / 2];
    }

    inline size_t threshold(const VId &n) {
        return 3 * n;
    }

    template<typename Edge>
    inline auto filter(std::span<Edge> edges, UnionFind &uf) {
        auto it = std::remove_if(edges.begin(), edges.end(), [&](const auto &edge) {
            VId f1 = uf.find(edge.get_src());
            VId f2 = uf.find(edge.get_dst());
            return f1 == f2;
        });
        return std::span<Edge>{edges.begin(), it};
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
        bigger = filter(bigger, uf);
        filterKruskal::getMST(n, bigger, mst, uf);
    }

    /**
     *  initial call to filter kruskal
     */
    template<typename Edge>
    inline std::vector<Edge> getMST(VId n, std::vector<Edge> &edges, UnionFind &uf) {
        std::vector<Edge> mst;
        mst.reserve(n);
        std::span<Edge> e(edges.begin(), edges.end());

        filterKruskal::getMST(n, e, mst, uf);
        return mst;
    }

}

