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


void runBoxplot(Config config, VId n, WEdgeList distEdges, std::vector<size_t> &numEdges,
                std::vector<size_t> &numVertices) {
    WEdgeOriginList distOriginEdges;

    if (config.algo == "boruvka" || config.algo == "mixedMerge" || config.algo == "boruvkaMerge") {
        for (auto &edge: distEdges) {
            distOriginEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
        }
    }

    numVertices.push_back(n);
    numEdges.push_back(distEdges.size());

    if (config.algo == "boruvka") {
        boruvka_allreduce::getBoxplot(n, distOriginEdges, config.localMSTcount, numEdges, numVertices,
                                      config.boruvkaThreadCount, config.useKruskal);
    } else if (config.algo == "merge") {
        mergeMST::getBoxplot(n, distEdges, numEdges, numVertices, config.useKruskal, config.treeFactor);
    } else if (config.algo == "mixedMerge") {
        mixed_merge::getBoxplot(n, distOriginEdges, config.localMSTcount, numEdges, numVertices, config.useKruskal,
                                config.treeFactor);
    } else if (config.algo == "boruvkaMerge") {
        boruvka_then_merge::getBoxplot(n, distOriginEdges, config.localMSTcount, numEdges, numVertices,
                                       config.useKruskal, config.treeFactor);
    }
}

std::pair<WEdgeList, VId>
runAlgorithm(Config config, VId n, WEdgeList allEdges, WEdgeList distEdges, WEdgeOriginList distOriginEdges,
             hybridMST::Timer &timer) {
    WEdgeList mst;
    NullTimer nullTimer = NullTimer();

    if (config.algo == "kruskal") { //run algo twice and ignore the first iteration
        UnionFind uf(n);
        timer.start(config.algo, 0);
        mst = kruskal::getMST(allEdges, uf);
        timer.stop(config.algo, 0);
    } else if (config.algo == "filter") {
        UnionFind uf(n);
        timer.start(config.algo, 0);
        mst = filterKruskal::getMST(n, allEdges, uf);
        timer.stop(config.algo, 0);
    } else if (config.algo == "boruvka") {
        if (config.onlyThisAlgo) {
            timer.start(config.algo, 0);
            mst = boruvka_allreduce::getMST(n, distOriginEdges, config.localMSTcount, timer, config.boruvkaThreadCount,
                                            config.useKruskal);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = boruvka_allreduce::getMST(n, distOriginEdges, config.localMSTcount, nullTimer, config.boruvkaThreadCount,
                                            config.useKruskal);
            timer.stop(config.algo, 0);
        }
    } else if (config.algo == "merge") {
        if (config.onlyThisAlgo) {
            timer.start(config.algo, 0);
            mst = mergeMST::getMST(n, distEdges, config.useKruskal, timer, config.treeFactor);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = mergeMST::getMST(n, distEdges, config.useKruskal, nullTimer, config.treeFactor);
            timer.stop(config.algo, 0);
        }


    } else if (config.algo == "mixedMerge") {
        if (config.onlyThisAlgo) {
            timer.start(config.algo, 0);
            mst = mixed_merge::getMST(n, distOriginEdges, config.localMSTcount, timer, config.useKruskal,
                                      config.treeFactor);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = mixed_merge::getMST(n, distOriginEdges, config.localMSTcount, nullTimer, config.useKruskal,
                                      config.treeFactor);
            timer.stop(config.algo, 0);
        }
    } else if (config.algo == "boruvkaMerge") {
        if (config.onlyThisAlgo) {
            timer.start(config.algo, 0);
            mst = boruvka_then_merge::getMST(n, distOriginEdges, config.localMSTcount, timer, config.useKruskal,
                                             config.treeFactor);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = boruvka_then_merge::getMST(n, distOriginEdges, config.localMSTcount, nullTimer, config.useKruskal,
                                             config.treeFactor);
            timer.stop(config.algo, 0);
        }

    }

    VId weight = 0;
    for (auto &edge: mst) {
        weight += edge.get_weight();
    }
    return {mst, weight};
}


std::pair<WEdgeList, VId> runKruskal(VId n, WEdgeList allEdges) {
    UnionFind uf(n);
    WEdgeList mst = kruskal::getMST(allEdges, uf);
    VId kruskalWeight = 0;
    for (auto &edge: mst) {
        kruskalWeight += edge.get_weight();
    }
    return {mst, kruskalWeight};
}