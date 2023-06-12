#pragma once

#include "../external/graphs/interface.hpp"
#include "../include/mpi/gather.hpp"

namespace generateGraph {


    template<typename Edge>
    std::vector<Edge> getDistEdges(Config &config) {
        //TODO: get algo from config


        if (config.graphType == "rhg") {
            auto [distEdges, vertex_range] = graphs::get_rhg(config.log_n, config.log_m, 10);
            return distEdges;
        } else if (config.graphType == "gnm") {
            auto [distEdges, vertex_range] = graphs::get_gnm(config.log_n, config.log_m);
            return distEdges;
        } else {
            std::cout << "wrong graph type" << std::endl;
        }


        return {};
    }

    template<typename Edge>
    std::vector<Edge> getAllEdges(std::vector<Edge> &distEdges) {
        hybridMST::mpi::MPIContext ctx;
        return hybridMST::mpi::gatherv(distEdges.data(), distEdges.size(), 0, ctx);

    }


}