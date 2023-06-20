#pragma once

#include "../include/definitions.hpp"
#include "generateGraph.hpp"
#include "executeAlgorithm.hpp"
#include "../algorithms/mergeMST.hpp"
#include "csvWriter.hpp"

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


    void executeCommand(Config &config) {
        hybridMST::mpi::MPIContext ctx;
        WEdgeList distEdges = generateGraph::getDistEdges<WEdge>(config);
        WEdgeList allEdges = generateGraph::getAllEdges(distEdges);
        if (ctx.rank() == 0) {
            std::cout << "graph has been generated" << std::endl;
        }

        VId n = pow(2, config.log_n);
        hybridMST::Timer timer;

        auto [mst, w] = runAlgorithm(config, n, allEdges, distEdges, timer);
        if (ctx.rank() == 0 && config.test) {
            auto [_, kruskalWeight] = runKruskal(n, allEdges);
            checkWeights(w, kruskalWeight, config.algo);
        }



        std::cout << timer.output();

        std::string timerOutput = timer.output();

        if (ctx.rank() == 0) {
            std::cout << std::endl;

            std::string filePath = "out/files/";

            writer::write_csv(filePath, config, timerOutput);
            std::cout << "results have been written to " << filePath << std::endl;
        }
    }
}