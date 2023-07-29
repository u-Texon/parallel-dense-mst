#pragma once


#include <span>
#include "../include/dataStructures/union_find.hpp"
#include "../include/definitions.hpp"
#include "ips4o.hpp"


namespace kruskal {

    /* TODO remove at some point
     * #include <chrono>
    template<typename Edge>
    inline std::vector<Edge> getMST(std::vector<Edge> &edges, UnionFind &uf) {
        auto t1 = std::chrono::high_resolution_clock::now();
        ips4o::sort(edges.begin(), edges.end(), WeightOrder<Edge>{});
        auto t2 = std::chrono::high_resolution_clock::now();
        std::vector<Edge> mst_edges;
        for (auto &elem: edges) {
            if (uf.find(elem.get_src()) == uf.find(elem.get_dst())) continue;
            mst_edges.push_back(elem);
            uf.unify(elem.get_src(), elem.get_dst());
        }
        auto t3 = std::chrono::high_resolution_clock::now();
        return mst_edges;
    }
     */

    template<typename Edge>
    inline std::vector<Edge> getMST(std::vector<Edge> &edges, UnionFind &uf) {
        ips4o::sort(edges.begin(), edges.end(), WeightOrder<Edge>{});
        std::vector<Edge> mst_edges;
        for (auto &elem: edges) {
            if (uf.find(elem.get_src()) == uf.find(elem.get_dst())) continue;
            mst_edges.push_back(elem);
            uf.unify(elem.get_src(), elem.get_dst());
        }
        return mst_edges;
    }


    template<typename Edge>
    inline void getMST(std::span<Edge> &edges, std::vector<Edge> &mst, UnionFind &uf) {
        ips4o::sort(edges.begin(), edges.end(), WeightOrder<Edge>{});
        for (auto &elem: edges) {
            if (uf.find(elem.get_src()) == uf.find(elem.get_dst())) continue;
            mst.push_back(elem);
            uf.unify(elem.get_src(), elem.get_dst());
        }
    }


}  // namespace hybridMST
