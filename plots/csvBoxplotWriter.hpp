#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>

namespace writer {

    void writeBoxplot(const std::string &filePath, Config &config, std::vector<size_t> &numEdges,
                      std::vector<size_t> &numVertices) {

        hybridMST::mpi::MPIContext ctx;
        std::string fileName = filePath + config.algo + "_boxplot-" + std::to_string(ctx.size()) + ".csv";


        std::ifstream f(fileName);
        bool alreadyExists = f.good();
        if (alreadyExists) {
            return; //no need to write boxplot more than once
        }

        ctx.barrier();

        std::ofstream file;
        if (ctx.rank() == 0) {
            std::filesystem::remove(fileName);
            file.open(fileName);
            if (!file.is_open()) {
                std::cout << "!!! error on opening file " << fileName << " !!!" << std::endl;
            }
            file << "Algorithm,Processor Number,Iteration,NumEdges,numVertices" << std::endl;
        }
        ctx.barrier();
        if (ctx.rank() != 0) {
            file.open(fileName, std::ios::app);
            if (!file.is_open()) {
                std::cout << "!!! error on opening file " << fileName << " !!!" << std::endl;
            }
        }


        for (int i = 0; i < numEdges.size(); ++i) {
            if (i >= numVertices.size()) {
                file << config.algo << "," << ctx.rank() << "," << i << "," << numEdges[i] << "," << 0
                     << std::endl;
            } else {
                file << config.algo << "," << ctx.rank() << "," << i << "," << numEdges[i] << "," << numVertices[i]
                     << std::endl;
            }
        }

        if (ctx.rank() == 0) {
            std::cout << "Boxplot-results have been written to " << filePath << std::endl;
        }

    }
}