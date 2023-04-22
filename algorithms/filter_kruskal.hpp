#pragma once

#include "kruskal.hpp"

namespace filterKruskal {

    inline WEdge pickPivot(WEdgeList &edges) {
        return edges[edges.size() / 2];
    }

    inline int threshold() {
        return 10; //TODO: determine threshold;
    }

    inline WEdgeList filter(WEdgeList &edges, UnionFind *uf) {
        WEdgeList newList;
        for (auto edge: edges) {
            int f1 = uf->find(edge.get_src());
            int f2 = uf->find(edge.get_dst());
            if (f1 != f2) {
                newList.push_back(edge);
            }
        }
        return newList;
    }


    inline WEdgeList getMST(WEdgeList &edges, UnionFind *uf) {
        if (edges.size() <= threshold()) {
            return kruskal::getMST(edges, uf);
        }

        WEdge pivot = pickPivot(edges);
        WEdgeList smaller;
        WEdgeList bigger;
        for (auto edge: edges) {
            if (edge.get_weight() <= pivot.get_weight()) {
                smaller.push_back(edge);
            } else {
                bigger.push_back(edge);
            }
        }
        assert(smaller.size() + bigger.size() == edges.size());
        WEdgeList mst;
        mst = filterKruskal::getMST(smaller, uf);
        bigger = filter(bigger, uf);
        WEdgeList bigEdges = filterKruskal::getMST(bigger, uf);
        mst.insert(mst.end(), bigEdges.begin(), bigEdges.end());

        return mst;
    }


}