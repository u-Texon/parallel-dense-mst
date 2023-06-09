#pragma once

#include <iostream>
#include "../include/util/timer.hpp"
#include "../algorithms/kruskal.hpp"
#include "../include/definitions.hpp"
#include "../algorithms/filter_kruskal.hpp"
#include "../algorithms/mergeMST.hpp"
#include "../algorithms/dense_boruvka.hpp"
#include "../algorithms/mixedMerge.hpp"
#include "../algorithms/boruvka_then_merge.hpp"



std::pair<WEdgeList, VId> testMergeMST(VId &n, WEdgeList &edges, hybridMST::Timer &timer) {
    timer.start("merge", 0);
    WEdgeList mergeMst = mergeMST::getMST(n, edges);
    timer.stop("merge", 0);
    VId mergeWeight = 0;
    for (auto &edge: mergeMst) {
        mergeWeight += edge.get_weight();
    }
    std::pair<WEdgeList, VId> pair;
    pair.first = mergeMst;
    pair.second = mergeWeight;
    return pair;
}


std::pair<WEdgeList, VId> testKruskal(VId &n, WEdgeList &edges, hybridMST::Timer &timer) {
    timer.start("kruskal", 0);
    UnionFind uf(n);
    WEdgeList mst = kruskal::getMST(edges, uf);
    timer.stop("kruskal", 0);
    VId kruskalWeight = 0;
    for (auto &edge: mst) {

        kruskalWeight += edge.get_weight();
    }
    std::pair<WEdgeList, VId> pair(mst, kruskalWeight);
    return pair;
}

std::pair<WEdgeList, VId> testFilterKruskal(VId &n, WEdgeList &edges, hybridMST::Timer &timer) {
    timer.start("filter", 0);
    UnionFind uf(n);
    VId c = 0;
    std::vector<WEdge> mst = filterKruskal::getMST(n, edges, uf, c);
    timer.stop("filter", 0);

    VId filterWeight = 0;
    for (auto &edge: mst) {
        filterWeight += edge.get_weight();
    }

    std::pair<WEdgeList, VId> pair(mst, filterWeight);
    return pair;
}


std::pair<WEdgeList, VId> testDenseBoruvka(VId &n, WEdgeList &edges, hybridMST::Timer &timer) {

    WEdgeOriginList newEdges;
    for (auto &edge: edges) {
        newEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
    }

    timer.start("denseBoruvka", 0);
    std::vector<WEdge> mst = dense_boruvka::getMST(n, newEdges);
    timer.stop("denseBoruvka", 0);

    VId bWeight = 0;
    for (auto &edge: mst) {
        bWeight += edge.get_weight();
    }
    std::pair<WEdgeList, VId> pair(mst, bWeight);
    return pair;
}


std::pair<WEdgeList, VId> testMixedMerge(VId &n, WEdgeList &edges, hybridMST::Timer &timer) {

    WEdgeOriginList newEdges;
    for (auto &edge: edges) {
        newEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
    }

    timer.start("mixedMerge", 0);
    std::vector<WEdge> mst = mixed_merge::getMST(n, newEdges);
    timer.stop("mixedMerge", 0);

    VId mmWeight = 0;
    for (auto &edge: mst) {
        mmWeight += edge.get_weight();
    }
    std::pair<WEdgeList, VId> pair(mst, mmWeight);
    return pair;
}

std::pair<WEdgeList, VId> testBoruvkaThenMerge(VId &n, WEdgeList &edges, hybridMST::Timer &timer) {

    WEdgeOriginList newEdges;
    for (auto &edge: edges) {
        newEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
    }

    timer.start("boruvkaThenMerge", 0);
    std::vector<WEdge> mst = boruvka_then_merge::getMST(n, newEdges);
    timer.stop("boruvkaThenMerge", 0);

    VId btmWeight = 0;
    for (auto &edge: mst) {
        btmWeight += edge.get_weight();
    }
    std::pair<WEdgeList, VId> pair(mst, btmWeight);
    return pair;
}