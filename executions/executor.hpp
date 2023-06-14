#pragma once

#include "../include/definitions.hpp"
#include "generateGraph.hpp"
#include "tests/tests.hpp"
#include "../algorithms/mergeMST.hpp"


namespace executor {

    void executeCommand(Config &config) {
        hybridMST::mpi::MPIContext ctx;


        WEdgeList distEdges = generateGraph::getDistEdges<WEdge>(config);
        WEdgeList allEdges = generateGraph::getAllEdges(distEdges);


        VId n = pow(2, config.log_n);


        if (ctx.rank() == 0) {
            std::cout << "calculations start!" << std::endl;
        }

        hybridMST::Timer timer;


        if (config.algo == "kruskal") {
            auto [kruskalMST, kruskalWeight] = testKruskal(n, allEdges, timer, config.test);
        } else if (config.algo == "boruvka") {
            auto [denseBoruvkaMST, bWeight] = testDenseBoruvka(n, distEdges, timer, config.test);
        } else if (config.algo == "merge") {
            auto [mergeMST, mergeWeight] = testMergeMST(n, distEdges, timer, config.test);
        } else if (config.algo == "filter") {
            auto [filterMST, filterWeight] = testFilterKruskal(n, allEdges, timer, config.test);
        } else if (config.algo == "mixedMerge") {
            auto [mixedMergeMST, mmWeight] = testMixedMerge(n, distEdges, timer, config.test);
        } else if (config.algo == "boruvkaMerge") {
            auto [boruvkaThenMergeMST, btmWeight] = testBoruvkaThenMerge(n, distEdges, timer, config.test);
        } else {
            if (ctx.rank() == 0) {
                std::cout << "no such algorithm" << std::endl;
            }
            return;
        }


        /*
        if (config.test) {
            if (ctx.rank() == 0) { //local tests

                if (kruskalWeight != btmWeight) {
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                    std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
                    std::cout << "boruvka-then-merge found MST with weight: " << btmWeight << std::endl;
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                }


                if (kruskalWeight != mmWeight) {
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                    std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
                    std::cout << "mixedMerge found MST with weight: " << mmWeight << std::endl;
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                }

                if (kruskalWeight != bWeight) {
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                    std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
                    std::cout << "dense boruvka found MST with weight: " << bWeight << std::endl;
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                }

                if (kruskalWeight != filterWeight) {
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                    std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
                    std::cout << "filter-kruskal found MST with weight: " << filterWeight << std::endl;
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                }
                if (kruskalWeight != mergeWeight) {
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                    std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
                    std::cout << "mergeMST found MST with weight: " << mergeWeight << std::endl;
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                }

                std::cout << "calculations complete!" << std::endl;
            }
        } else {
            if (ctx.rank() == 0) {
                std::cout << "no test have been run" << std::endl;
            }
        } */


        std::cout << timer.output();
        if (ctx.rank() == 0) {
            std::cout << std::endl;
        }

    }
}