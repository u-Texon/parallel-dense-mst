#pragma once

#include "../include/definitions.hpp"
#include "generateGraph.hpp"
#include "executeAlgorithm.hpp"
#include "../algorithms/mergeMST.hpp"
#include "../plots/csvWriter.hpp"
#include <filesystem>
#include "../commands/commandParser.hpp"

namespace executor {
    void checkWeights(VId kruskalW, VId algoW, const std::string &algoName) {
        if (kruskalW != algoW) {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "kruskal found MST with weight: " << kruskalW << std::endl;
            std::cout << algoName << " found MST with weight: " << algoW << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        } else {
            std::cout << algoName << " got an mst with the correct weight of " << algoW << std::endl;
        }
    }


    void executeAlgorithm(Config &config, WEdgeList &distEdges, WEdgeList &allEdges, hybridMST::Timer &timer) {
        hybridMST::mpi::MPIContext ctx;

        VId n = pow(2, config.log_n);

        auto [mst, algoWeight] = runAlgorithm(config, n, allEdges, distEdges, timer);
        if (ctx.rank() == 0 && config.test) {
            auto [_, kruskalWeight] = runKruskal(n, allEdges);
            checkWeights(kruskalWeight, algoWeight, config.algo);
        }


        std::cout << timer.output();

        std::string timerOutput = timer.output();

        if (ctx.rank() == 0) {
            std::cout << std::endl;


            std::string filePath = "out/files";
            std::filesystem::create_directories(filePath);
            std::filesystem::create_directory("out/plots");
            filePath += "/";


            writer::write_csv(filePath, config, timerOutput);
            std::cout << "results have been written to " << filePath << std::endl;
        }
    }

    void executeCommand(Config &config) {
        hybridMST::mpi::MPIContext ctx;
        WEdgeList distEdges = generateGraph::getDistEdges<WEdge>(config);
        WEdgeList allEdges = distEdges;

        if (config.algo == "kruskal" || config.algo == "all" || config.algo == "filter") {
            allEdges = generateGraph::getAllEdges(distEdges);
        }
        if (ctx.rank() == 0) {
            std::cout << "graph has been generated" << std::endl;
        }


        if (config.algo == "all" || config.algo == "allParallel") {
            std::vector<std::string> algorithms = {"boruvka", "mixedMerge", "merge", "boruvkaMerge"};
            if (config.algo == "all") {
                algorithms.push_back("kruskal");
                algorithms.push_back("filter");
            }
            for (const auto &algo: algorithms) {
                config.algo = algo;
                hybridMST::Timer timer;
                executeAlgorithm(config, distEdges, allEdges, timer);
            }
        } else {
            hybridMST::Timer timer;
            executeAlgorithm(config, distEdges, allEdges, timer);
        }
    }
}