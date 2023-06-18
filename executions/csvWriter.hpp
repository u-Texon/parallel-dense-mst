#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

namespace writer {


    void write_csv(const std::string &filename, Config &config, const std::string &result) {
        hybridMST::mpi::MPIContext ctx;
        std::ofstream file;

        std::ifstream f(filename);
        bool alreadyExists = f.good();

        file.open(filename, std::ios::app);
        if (!file.is_open()) {
            std::cout << "!!! error on opening file " << filename << " !!!" << std::endl;
        }


        if (!alreadyExists) {
            file << "Algorithm,Processors,log(m),log(n),minimum weight,maximum weight,graph-type,tree-factor,run time\n";
        }
        file << config.algo << "," << ctx.size() << "," << config.log_m << "," << config.log_n << "," << config.minWeight << ","
             << config.maxWeight << "," << config.graphType << "," << config.treeFactor << "," << result << std::endl;
        file.close();

    }


}