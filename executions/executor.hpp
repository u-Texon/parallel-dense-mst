#pragma once

#include "../include/definitions.hpp"
#include "generateGraph.hpp"
#include "tests/tests.hpp"
#include "../algorithms/mergeMST.hpp"


namespace executor {


    void checkWeights(VId kruskalW, VId algoW, const std::string &algoName) {
        hybridMST::mpi::MPIContext ctx;
        if (ctx.rank() == 0) {
            if (kruskalW != algoW) {
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                std::cout << "kruskal found MST with weight: " << kruskalW << std::endl;
                std::cout << algoName << " found MST with weight: " << algoW << std::endl;
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            } else {
                std::cout << algoName << " got an mst with the correct weight of " << algoW << std::endl;
            }
        }
    }


    void executeCommand(Config &config) {
        hybridMST::mpi::MPIContext ctx;



        if (ctx.rank() == 0) {
            std::cout << "calculations start!" << std::endl;
        }

        WEdgeList distEdges = generateGraph::getDistEdges<WEdge>(config);
        WEdgeList allEdges = generateGraph::getAllEdges(distEdges);


        if (ctx.rank() == 0) {
            std::cout << "graph has been generated" << std::endl;
        }

        VId n = pow(2, config.log_n);
        hybridMST::Timer timer;
        VId kruskalWeight = 0;

        if (config.algo == "kruskal" || config.test) {
            auto [mst, w] = testKruskal(n, allEdges, timer, config.test);
            kruskalWeight = w;
        }
        if (config.algo == "boruvka") {
            auto [mst, w] = testDenseBoruvka(n, distEdges, timer, config.test);
            if (config.test) {
                checkWeights(w, kruskalWeight, config.algo);
            }
        } else if (config.algo == "merge") {
            auto [mst, w] = testMergeMST(n, distEdges, timer, config.test, config.treeFactor);
            if (config.test) {
                checkWeights(w, kruskalWeight, config.algo);
            }
        } else if (config.algo == "filter") {
            auto [mst, w] = testFilterKruskal(n, allEdges, timer, config.test);
            if (config.test) {
                checkWeights(w, kruskalWeight, config.algo);
            }
        } else if (config.algo == "mixedMerge") {
            auto [mst, w] = testMixedMerge(n, distEdges, timer, config.test, config.treeFactor);
            if (config.test) {
                checkWeights(w, kruskalWeight, config.algo);
            }
        } else if (config.algo == "boruvkaMerge") {
            auto [mst, w] = testBoruvkaThenMerge(n, distEdges, timer, config.test, config.treeFactor);
            if (config.test) {
                checkWeights(w, kruskalWeight, config.algo);
            }
        } else {
            if (ctx.rank() == 0) {
                std::cout << "no such algorithm" << std::endl;
            }
            return;
        }

        if (!config.test && ctx.rank() == 0) {
            std::cout << "no test have been run" << std::endl;
        }


        std::cout << timer.output();
        if (ctx.rank() == 0) {
            std::cout << std::endl;
        }

    }
}