#pragma once

#include "kruskal.hpp"

#define maxLoops 10

namespace filterKruskal {

    template<typename Edge>
    inline Edge pickPivot(std::vector<Edge> &edges) { //TODO:alternative: pick 3 elements
        int i = rand() % ( edges.size());
        return edges.at(i);
    }

    inline int threshold(const VId &n) {
        int t = (int) fmax(30, (log2(n) / 2)); //TODO: is this a good threshold?
        return t;
    }

    template<typename Edge>
    inline std::vector<Edge> filter(std::vector<Edge> &edges, UnionFind &uf) {
        std::vector<Edge> newList;
        for (auto &edge: edges) {
            VId f1 = uf.find(edge.get_src());
            VId f2 = uf.find(edge.get_dst());
            if (f1 != f2) {
                newList.push_back(edge);
            }
        }
        return newList;
    }


    template<typename Edge>
    inline std::vector<Edge> getMST(VId n, std::vector<Edge> &edges, UnionFind &uf, VId &counter) {
        int t = threshold(n);
        if (edges.size() <= t) {
            return kruskal::getMST(edges, uf);
        }
        Edge pivot = pickPivot(edges);
        std::vector<Edge> smaller;
        std::vector<Edge> bigger;
        for (auto &edge: edges) {
            if (edge.get_weight() <= pivot.get_weight()) {
                smaller.push_back(edge);
            } else {
                bigger.push_back(edge);
            }
        }

        if (bigger.empty()) { //error prevention for infinite loop
            counter++;
            if (counter >= maxLoops) {
                return kruskal::getMST(smaller, uf);
            }
        } else {
            counter = 0;
        }

        std::vector<Edge> mst;
        mst = filterKruskal::getMST(n, smaller, uf, counter);
        bigger = filter(bigger, uf);
        std::vector<Edge> bigEdges = filterKruskal::getMST(n, bigger, uf, counter);
        mst.insert(mst.end(), bigEdges.begin(), bigEdges.end());

        return mst;
    }

}