#pragma once

namespace writer {


    void write_csv(const std::string &filename, Config &config, const std::string &result) {
        hybridMST::mpi::MPIContext ctx;
        std::ofstream file;
        file.open(filename + ".csv");

        if (!file.is_open()) {
            std::cout << "!!! error on opening file " << filename + ".csv" << " !!!" << std::endl;
        }


        file << "Algorithm, Processors, log(m), log(n), minimum weight, maximum weight, graph-type, tree-factor, run time\n";
        file << config.algo << "," << ctx.size() << "," << config.log_m << "," << config.log_n << "," << config.minWeight << ","
             << config.maxWeight << "," << config.graphType << "," << config.treeFactor << "," << result << std::endl;
        file.close();

    }


}