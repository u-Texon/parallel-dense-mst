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


std::pair<WEdgeList, VId>
runAlgorithm(Config &config, VId &n, WEdgeList &allEdges, WEdgeList &distEdges, hybridMST::Timer &timer) {
    hybridMST::mpi::MPIContext ctx;

    WEdgeList mst;
    WEdgeOriginList distOriginEdges;

    if (config.algo == "boruvka" || config.algo == "mixedMerge" || config.algo == "boruvkaMerge") {
        for (auto &edge: distEdges) {
            distOriginEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
        }
    }

    if (config.algo == "kruskal") { //run algo twice and ignore the first iteration
        UnionFind uf(n);
        mst = kruskal::getMST(allEdges, uf);
        uf.clear();
        timer.start(config.algo, 0);
        mst = kruskal::getMST(allEdges, uf);
        timer.stop(config.algo, 0);
    } else if (config.algo == "filter") {
        UnionFind uf(n);
        mst = filterKruskal::getMST(n, allEdges, uf);
        uf.clear();
        timer.start(config.algo, 0);
        mst = filterKruskal::getMST(n, allEdges, uf);
        timer.stop(config.algo, 0);
    } else if (config.algo == "boruvka") {
        mst = dense_boruvka::getMST(n, distOriginEdges, config.useKruskal);
        if (config.onlyThisAlgo) {
            timer.start(config.algo, 0);
            mst = dense_boruvka::getMST(n, distOriginEdges, config.useKruskal, timer);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = dense_boruvka::getMST(n, distOriginEdges, config.useKruskal);
            timer.stop(config.algo, 0);
        }
    } else if (config.algo == "merge") {
        mst = mergeMST::getMST(n, distEdges, config.useKruskal, config.treeFactor);
        timer.start(config.algo, 0);
        mst = mergeMST::getMST(n, distEdges, config.useKruskal, config.treeFactor);
        timer.stop(config.algo, 0);
    } else if (config.algo == "mixedMerge") {
        mst = mixed_merge::getMST(n, distOriginEdges, config.treeFactor);
        timer.start(config.algo, 0);
        mst = mixed_merge::getMST(n, distOriginEdges, config.treeFactor);
        timer.stop(config.algo, 0);
    } else if (config.algo == "boruvkaMerge") {
        mst = boruvka_then_merge::getMST(n, distOriginEdges, config.treeFactor);
        timer.start(config.algo, 0);
        mst = boruvka_then_merge::getMST(n, distOriginEdges, config.treeFactor);
        timer.stop(config.algo, 0);
    }

    VId weight = 0;

    if (config.test) {
        for (auto &edge: mst) {
            weight += edge.get_weight();
        }
    }
    return {mst, weight};
}


std::pair<WEdgeList, VId> runKruskal(VId &n, WEdgeList &allEdges) {
    UnionFind uf(n);
    WEdgeList mst = kruskal::getMST(allEdges, uf);
    VId kruskalWeight = 0;
    for (auto &edge: mst) {
        kruskalWeight += edge.get_weight();
    }
    return {mst, kruskalWeight};
}