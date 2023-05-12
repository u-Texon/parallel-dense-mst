#pragma once

#include "kruskal.hpp"

namespace filterKruskal {

    inline WEdge pickPivot(WEdgeList &edges) { //TODO:alternative: pick 3 elements
        int i = rand() % ( edges.size());
        return edges.at(i);
    }

    inline int threshold(const int &n) {
        int t = (int) fmax(30, (log2(n) / 2)); //TODO: is this a good threshold?
        return t;
    }

    inline WEdgeList filter(WEdgeList &edges, UnionFind &uf) {
        WEdgeList newList;
        for (auto &edge: edges) {
            VId f1 = uf.find(edge.get_src());
            VId f2 = uf.find(edge.get_dst());
            if (f1 != f2) {
                newList.push_back(edge);
            }
        }
        return newList;
    }


    inline WEdgeList getMST(int &n, WEdgeList &edges, UnionFind &uf) {
        int t = threshold(n);
        if (edges.size() <= t) {
            return kruskal::getMST(edges, uf);
        }
        WEdge pivot = pickPivot(edges);
        WEdgeList smaller;
        WEdgeList bigger;
        for (auto &edge: edges) {
            if (edge.get_weight() <= pivot.get_weight()) {
                smaller.push_back(edge);
            } else {
                bigger.push_back(edge);
            }
        }

        if (bigger.empty()) { //error prevention for infinite loop
            return kruskal::getMST(smaller, uf);
        }

        WEdgeList mst;
        mst = filterKruskal::getMST(n, smaller, uf);
        bigger = filter(bigger, uf);
        WEdgeList bigEdges = filterKruskal::getMST(n, bigger, uf);
        mst.insert(mst.end(), bigEdges.begin(), bigEdges.end());

        return mst;
    }


}