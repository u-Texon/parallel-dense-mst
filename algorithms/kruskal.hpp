#pragma once


#include "../include/dataStructures/union_find.hpp"
#include "../include/definitions.hpp"

namespace kruskal {

    template <typename EdgeType> struct WeightOrder {
        bool operator()(const EdgeType& lhs, const EdgeType& rhs) const {
            return lhs.get_weight() < rhs.get_weight();
        }
    };


    template<typename Compare = WeightOrder<WEdge>>
    inline WEdgeList getMST(WEdgeList &edges, UnionFind &uf) {
        std::sort(edges.begin(), edges.end(), Compare{});
        WEdgeList mst_edges;
        for (auto &elem: edges) {
            if (uf.find(elem.get_src()) == uf.find(elem.get_dst())) continue;
            mst_edges.push_back(elem);
            uf.unify(elem.get_src(), elem.get_dst());
        }
        return mst_edges;
    }


}  // namespace hybridMST
