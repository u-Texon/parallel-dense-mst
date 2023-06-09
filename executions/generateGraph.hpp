#pragma once

#include "../external/graphs/interface.hpp"
#include "../include/mpi/gather.hpp"

namespace generateGraph {


    template<typename Edge>
    std::vector<Edge> getDistEdges(Config &config) {
        //TODO: get algo from config
        auto [distEdges, vertex_range] = graphs::get_rhg_explicit_num_edges(config.log_n, config.log_m, 10);

        return distEdges;
    }

    template<typename Edge>
    std::vector<Edge> getAllEdges(std::vector<Edge> &distEdges) {
        hybridMST::mpi::MPIContext ctx;
        return hybridMST::mpi::gatherv(distEdges.data(), distEdges.size(), 0, ctx);

    }


}