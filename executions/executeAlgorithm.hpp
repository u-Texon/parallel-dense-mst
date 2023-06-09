#pragma once

#include <iostream>
#include "../include/util/timer.hpp"
#include "../algorithms/kruskal.hpp"
#include "../include/definitions.hpp"
#include "../algorithms/filter_kruskal.hpp"
#include "../algorithms/mergeMST.hpp"
#include "../algorithms/boruvka_allreduce.hpp"
#include "../algorithms/mixedMerge.hpp"
#include "../algorithms/boruvka_then_merge.hpp"


std::pair<WEdgeList, VId>
runAlgorithm(Config &config, VId &n, WEdgeList &allEdges, WEdgeList &distEdges, hybridMST::Timer &timer) {
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
        if (config.onlyThisAlgo) {
            mst = boruvka_allreduce::getMST(n, distOriginEdges, config.localMSTcount, timer, config.useKruskal);
            timer.start(config.algo, 0);
            mst = boruvka_allreduce::getMST(n, distOriginEdges, config.localMSTcount, timer, config.useKruskal);
            timer.stop(config.algo, 0);
        } else {
            mst = boruvka_allreduce::getMST(n, distOriginEdges, config.localMSTcount, NullTimer::getInstance(),
                                            config.useKruskal);
            timer.start(config.algo, 0);
            mst = boruvka_allreduce::getMST(n, distOriginEdges, config.localMSTcount, NullTimer::getInstance(),
                                            config.useKruskal);
            timer.stop(config.algo, 0);
        }
    } else if (config.algo == "merge") {
        if (config.onlyThisAlgo) {
            mst = mergeMST::getMST(n, distEdges, config.useKruskal, timer, config.treeFactor);
            timer.start(config.algo, 0);
            mst = mergeMST::getMST(n, distEdges, config.useKruskal, timer, config.treeFactor);
            timer.stop(config.algo, 0);
        } else {
            mst = mergeMST::getMST(n, distEdges, config.useKruskal, NullTimer::getInstance(), config.treeFactor);
            timer.start(config.algo, 0);
            mst = mergeMST::getMST(n, distEdges, config.useKruskal, NullTimer::getInstance(), config.treeFactor);
            timer.stop(config.algo, 0);
        }


    } else if (config.algo == "mixedMerge") {
        //mst = mixed_merge::getMST(n, distOriginEdges, config.localMSTcount, config.useKruskal, config.treeFactor);
        timer.start(config.algo, 0);
        mst = mixed_merge::getMST(n, distOriginEdges, config.localMSTcount, config.useKruskal, config.treeFactor);
        timer.stop(config.algo, 0);
    } else if (config.algo == "boruvkaMerge") {
        mst = boruvka_then_merge::getMST(n, distOriginEdges, config.localMSTcount, config.useKruskal,
                                         config.treeFactor);
        timer.start(config.algo, 0);
        mst = boruvka_then_merge::getMST(n, distOriginEdges, config.localMSTcount, config.useKruskal,
                                         config.treeFactor);
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