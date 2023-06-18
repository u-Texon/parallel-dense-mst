#pragma once

#include "../external/graphs/interface.hpp"
#include "../include/mpi/gather.hpp"

namespace generateGraph {

    //TODO: shuffle edges
    // kanten random ids geben und dann:
    //ams sort zum verteilen




    template<typename Edge>
    std::vector<Edge> getDistEdges(Config &config) {

        graphs::WeightGeneratorConfig<VId> weights;
        weights.min_weight = config.minWeight;
        weights.max_weight = config.maxWeight;

        if (config.graphType == "rhg") {
            auto [distEdges, vertex_range] = graphs::get_rhg(config.log_n, config.log_m, 3.0, weights);
            return distEdges;
        } else if (config.graphType == "gnm") {
            auto [distEdges, vertex_range] = graphs::get_gnm(config.log_n, config.log_m, weights);
            return distEdges;
        } else if (config.graphType == "rhgNumEdges") {
            auto [distEdges, vertex_range] = graphs::get_rhg_explicit_num_edges(config.log_n, config.log_m, 3.0, weights);
            return distEdges;
        }

        return {};
    }

    template<typename Edge>
    std::vector<Edge> getAllEdges(std::vector<Edge> &distEdges) {
        hybridMST::mpi::MPIContext ctx;
        return hybridMST::mpi::gatherv(distEdges.data(), distEdges.size(), 0, ctx);

    }


}