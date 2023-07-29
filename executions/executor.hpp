#pragma once

#include "../include/definitions.hpp"
#include "generateGraph.hpp"
#include "executeAlgorithm.hpp"
#include "../algorithms/mergeMST.hpp"
#include "../plots/csvWriter.hpp"
#include <filesystem>
#include "../commands/commandParser.hpp"
#include "../plots/csvBoxplotWriter.hpp"

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


    void executeAlgorithm(Config &config, WEdgeList &distEdges, WEdgeList &allEdges, WEdgeOriginList &distOriginEdges) {
        hybridMST::mpi::MPIContext ctx;
        VId n = pow(2, config.log_n);

        for (int i = 0; i < config.repeat; ++i) {
            hybridMST::Timer timer;
            auto [mst, algoWeight] = runAlgorithm(config, n, allEdges, distEdges, distOriginEdges, timer);
            if (ctx.rank() == 0 && config.test) {
                auto [_, kruskalWeight] = runKruskal(n, allEdges);
                checkWeights(kruskalWeight, algoWeight, config.algo);
            }
            if (i != 0) {
                std::string timerOutput = timer.output();
                if (ctx.rank() == 0) {
                    std::cout << timerOutput << std::endl;
                    std::cout << "mstWeight = " << algoWeight << std::endl;
                    std::string filePath = "out/files";
                    std::filesystem::create_directories(filePath);
                    std::filesystem::create_directory("out/plots");
                    filePath += "/";

                    writer::write_csv(filePath, config, timerOutput);
                    std::cout << "results have been written to " << filePath << std::endl;
                }
            }
        }
    }

    void makeBoxplot(Config &config, WEdgeList &distEdges) {
        hybridMST::mpi::MPIContext ctx;
        std::vector<size_t> numEdges;
        std::vector<size_t> numVertices;


        VId n = pow(2, config.log_n);
        runBoxplot(config, n, distEdges, numEdges, numVertices);

        std::string filePath = "out/files";
        std::filesystem::create_directories(filePath);
        std::filesystem::create_directory("out/plots");
        filePath += "/";

        writer::writeBoxplot(filePath, config, numEdges, numVertices);

        if (ctx.rank() == 0) {
            std::cout << "results have been written to " << filePath << std::endl;
        }

    }

    void executeCommand(Config &config) {
        hybridMST::mpi::MPIContext ctx;
        WEdgeList distEdges = generateGraph::getDistEdges<WEdge>(config);
        WEdgeList allEdges;
        WEdgeOriginList distOriginEdges;

        if (config.algo == "kruskal" || config.algo == "all" || config.algo == "filter" || config.test) {
            allEdges = generateGraph::getAllEdges(distEdges);
        }
        if (config.algo != "kruskal" && config.algo != "filter" && config.algo != "merge") {
            for (auto &edge: distEdges) {
                distOriginEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
            }
        }
        if (ctx.rank() == 0) {
            std::cout << "graph has been generated" << std::endl;
        }
        if (config.algo == "all" || config.algo == "allParallel") {
            std::vector<std::string> algorithms = {"boruvka", "mixedMerge", "merge", "boruvkaMerge"};
            if (config.algo == "all") {
                algorithms.emplace_back("kruskal");
                algorithms.emplace_back("filter");
            }
            for (const auto &algo: algorithms) {
                config.algo = algo;
                if (config.boxplot) {
                    makeBoxplot(config, distEdges);
                } else {
                    executeAlgorithm(config, distEdges, allEdges, distOriginEdges);
                }
            }
        } else {
            if (config.boxplot) {
                makeBoxplot(config, distEdges);
            } else {
                executeAlgorithm(config, distEdges, allEdges, distOriginEdges);
            }
        }
    }
}