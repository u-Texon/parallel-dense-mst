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


template<typename Algorithm>
std::pair<WEdgeList, VId>
testAlgorithmOriginEdges(Algorithm algorithm, std::string &algoName, VId &n, WEdgeList &edges,
                         hybridMST::Timer &timer, bool test) {
    //TODO: maybe use this

    WEdgeList mst;
    WEdgeOriginList newEdges;
    for (auto &edge: edges) {
        newEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
    }
    timer.start(algoName, 0);
    mst = algorithm(n, newEdges);
    timer.stop(algoName, 0);


    VId weight = 0;
    if (test) {
        for (auto &edge: mst) {
            weight += edge.get_weight();
        }
    }
    return {mst, weight};
}

template<typename Algorithm>
std::pair<WEdgeList, VId>
testAlgorithm(Algorithm algorithm, std::string &algoName, VId &n, WEdgeList &edges, hybridMST::Timer &timer,
              bool test) {
    WEdgeList mst;

    timer.start(algoName, 0);
    mst = algorithm(n, edges);
    timer.stop(algoName, 0);


    VId weight = 0;
    if (test) {
        for (auto &edge: mst) {
            weight += edge.get_weight();
        }
    }
    return {mst, weight};
}


std::pair<WEdgeList, VId> testMergeMST(VId &n, WEdgeList &edges, hybridMST::Timer &timer, bool test, VId treeFactor) {
    timer.start("merge", 0);
    WEdgeList mergeMst = mergeMST::getMST(n, edges, treeFactor);
    timer.stop("merge", 0);
    VId mergeWeight = 0;
    if (test) {
        for (auto &edge: mergeMst) {
            mergeWeight += edge.get_weight();
        }
    }
    return {mergeMst, mergeWeight};
}


std::pair<WEdgeList, VId> testKruskal(VId &n, WEdgeList &edges, hybridMST::Timer &timer, bool test) {
    timer.start("kruskal", 0);
    UnionFind uf(n);
    WEdgeList mst = kruskal::getMST(edges, uf);
    timer.stop("kruskal", 0);
    VId kruskalWeight = 0;
    if (test) {
        for (auto &edge: mst) {
            kruskalWeight += edge.get_weight();
        }
    }
    return {mst, kruskalWeight};
}

std::pair<WEdgeList, VId> testFilterKruskal(VId &n, WEdgeList &edges, hybridMST::Timer &timer, bool test) {
    timer.start("filter", 0);
    UnionFind uf(n);
    VId c = 0;
    std::vector<WEdge> mst = filterKruskal::getMST(n, edges, uf, c);
    timer.stop("filter", 0);

    VId filterWeight = 0;
    if (test) {
        for (auto &edge: mst) {
            filterWeight += edge.get_weight();
        }
    }
    return {mst, filterWeight};
}


std::pair<WEdgeList, VId> testDenseBoruvka(VId &n, WEdgeList &edges, hybridMST::Timer &timer, bool test) {

    WEdgeOriginList newEdges;
    for (auto &edge: edges) {
        newEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
    }

    timer.start("denseBoruvka", 0);
    std::vector<WEdge> mst = dense_boruvka::getMST(n, newEdges);
    timer.stop("denseBoruvka", 0);

    VId bWeight = 0;
    if (test) {
        for (auto &edge: mst) {
            bWeight += edge.get_weight();
        }
    }
    return {mst, bWeight};
}


std::pair<WEdgeList, VId> testMixedMerge(VId &n, WEdgeList &edges, hybridMST::Timer &timer, bool test, VId treeFactor) {

    WEdgeOriginList newEdges;
    for (auto &edge: edges) {
        newEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
    }

    timer.start("mixedMerge", 0);
    std::vector<WEdge> mst = mixed_merge::getMST(n, newEdges, treeFactor);
    timer.stop("mixedMerge", 0);

    VId mmWeight = 0;
    if (test) {
        for (auto &edge: mst) {
            mmWeight += edge.get_weight();
        }
    }
    return {mst, mmWeight};
}

std::pair<WEdgeList, VId> testBoruvkaThenMerge(VId &n, WEdgeList &edges, hybridMST::Timer &timer, bool test, VId treeFactor) {

    WEdgeOriginList newEdges;
    for (auto &edge: edges) {
        newEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
    }

    timer.start("boruvkaThenMerge", 0);
    std::vector<WEdge> mst = boruvka_then_merge::getMST(n, newEdges, treeFactor);
    timer.stop("boruvkaThenMerge", 0);

    VId btmWeight = 0;

    if (test) {
        for (auto &edge: mst) {
            btmWeight += edge.get_weight();
        }
    }

    return {mst, btmWeight};
}