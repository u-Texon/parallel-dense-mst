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


void filterEdges(WEdgeList &distEdges) {
    ips4o::sort(distEdges.begin(), distEdges.end(), SrcDstOrder<WEdge>{});
    auto it = std::unique(distEdges.begin(), distEdges.end(), [&](const auto &edge1, const auto &edge2) {
        if (edge1.src == edge2.src) {
            return edge1.dst == edge2.dst;
        }
        if (edge1.src == edge2.dst) {
            return edge1.dst == edge2.src;
        }
        return false;
    });
    distEdges.erase(it, distEdges.end());
}

std::pair<WEdgeList, VId>
runAlgorithm(Config config, VId n, WEdgeList allEdges, WEdgeList distEdges, std::vector<size_t> &numEdges,
             std::vector<size_t> &numVertices, WEdgeOriginList distOriginEdges, hybridMST::Timer &timer) {
    WEdgeList mst;
    NullTimer nullTimer = NullTimer();


    if (config.filterEdges) {
        timer.start("filterEdges", 0);
        filterEdges(distEdges);
        timer.stop("filterEdges", 0);
    }

    numVertices.push_back(n);
    numEdges.push_back(distEdges.size());

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
            mst = boruvka_allreduce::getMST(n, distOriginEdges, config.localMSTcount, numEdges, numVertices, timer,
                                            config.boruvkaOverlapCount, config.removeParallelEdges, config.useKruskal);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = boruvka_allreduce::getMST(n, distOriginEdges, config.localMSTcount, numEdges, numVertices, nullTimer,
                                            config.boruvkaOverlapCount, config.removeParallelEdges, config.useKruskal);
            timer.stop(config.algo, 0);
        }
    } else if (config.algo == "merge") {
        if (config.onlyThisAlgo) {
            timer.start(config.algo, 0);
            mst = mergeMST::getMST(n, distEdges, config.useKruskal, numEdges, numVertices, timer, config.treeFactor);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = mergeMST::getMST(n, distEdges, config.useKruskal, numEdges, numVertices, nullTimer,
                                   config.treeFactor);
            timer.stop(config.algo, 0);
        }


    } else if (config.algo == "mixedMerge") {
        if (config.onlyThisAlgo) {
            timer.start(config.algo, 0);
            mst = mixed_merge::getMST(n, distOriginEdges, config.localMSTcount, numEdges, numVertices, timer,
                                      config.removeParallelEdges, config.useKruskal, config.treeFactor);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = mixed_merge::getMST(n, distOriginEdges, config.localMSTcount, numEdges, numVertices, nullTimer,
                                      config.removeParallelEdges, config.useKruskal, config.treeFactor);
            timer.stop(config.algo, 0);
        }
    } else if (config.algo == "boruvkaMerge") {
        if (config.onlyThisAlgo) {
            timer.start(config.algo, 0);
            mst = boruvka_then_merge::getMST(n, distOriginEdges, config.localMSTcount, numEdges, numVertices, timer,
                                             config.removeParallelEdges,
                                             config.useKruskal,
                                             config.treeFactor);
            timer.stop(config.algo, 0);
        } else {
            timer.start(config.algo, 0);
            mst = boruvka_then_merge::getMST(n, distOriginEdges, config.localMSTcount, numEdges, numVertices, nullTimer,
                                             config.removeParallelEdges,
                                             config.useKruskal,
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