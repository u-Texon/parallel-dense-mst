#pragma once


#include <span>
#include "../include/dataStructures/union_find.hpp"
#include "../include/definitions.hpp"
#include "ips4o.hpp"

namespace kruskal {

    template<typename EdgeType>
    struct WeightOrder {
        bool operator()(const EdgeType &lhs, const EdgeType &rhs) const {
            return lhs.get_weight() < rhs.get_weight();
        }
    };


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
        std::vector<Edge> mst_edges;
        for (auto &elem: edges) {
            if (uf.find(elem.get_src()) == uf.find(elem.get_dst())) continue;
            mst_edges.push_back(elem);
            uf.unify(elem.get_src(), elem.get_dst());
        }
    }


}  // namespace hybridMST
