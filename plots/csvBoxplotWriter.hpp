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

        std::string fileName = filePath + config.algo + "_boxplot" + ".csv";

        std::ofstream file;
        if (ctx.rank() == 0) {
            std::filesystem::remove(fileName);
            file.open(fileName, std::ios::app);
            if (!file.is_open()) {
                std::cout << "!!! error on opening file " << fileName << " !!!" << std::endl;
            }
            file << "Algorithm,Processor Number,Iteration,numVertices,NumEdges" << std::endl;
        }
        ctx.barrier();

        if (ctx.rank() != 0) {
            file.open(fileName, std::ios::app);
            if (!file.is_open()) {
                std::cout << "!!! error on opening file " << fileName << " !!!" << std::endl;
            }
        }


        for (int i = 0; i < numVertices.size(); ++i) {
            file << config.algo << "," << ctx.rank() << "," << i << "," << numVertices[i] << "," << numEdges[i]
                 << std::endl;
        }


    }


}