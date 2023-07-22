#pragma once

#include "../external/graphs/interface.hpp"
#include "../include/mpi/gather.hpp"

#include "AmsSort/AmsSort.hpp"


namespace generateGraph {


    template<typename EdgeType>
    struct ShuffleOrder {
        bool operator()(const EdgeType &left, const EdgeType &right) const {
            return left.second < right.second;
        }
    };

    template<typename Edge>
    void distributedShuffle(std::vector<Edge> &edges) {
        hybridMST::mpi::MPIContext ctx;

        std::vector<std::pair<Edge, std::size_t>> shuffleEdges;
        for (auto &edge: edges) {
            shuffleEdges.push_back({edge, rand() % (ctx.size())});
        }
        hybridMST::mpi::TypeMapper<std::pair<Edge, std::size_t>> mapper;
        std::mt19937_64 gen(ctx.rank());

        //TODO: get rid of the warning
        Ams::sortLevel(mapper.get_mpi_datatype(), shuffleEdges, 2, gen, ctx.communicator(), ShuffleOrder<std::pair<Edge, std::size_t>>{});



        edges.clear();
        for (auto &edge: shuffleEdges) {
            edges.push_back(edge.first);
        }



    }


    template<typename Edge>
    std::vector<Edge> getDistEdges(Config &config) {
        hybridMST::mpi::MPIContext ctx;
        graphs::WeightGeneratorConfig<VId> weights;
        weights.min_weight = config.minWeight;
        weights.max_weight = config.maxWeight;

        std::vector<graphs::WEdge24> distEdges;

        VId numEdges = config.log_m;

        if (config.edgesPerProc) {
            numEdges = config.edgesPerProc + log2(ctx.size());
        }

        if (config.graphType == "rhg") {
            auto [edges, vertex_range] = graphs::get_rhg(config.log_n, numEdges, 3.0, weights);
            distEdges = edges;
        } else if (config.graphType == "gnm") {
            auto [edges, vertex_range] = graphs::get_gnm(config.log_n, numEdges, weights);
            distEdges = edges;
        } else if (config.graphType == "rhgNumEdges") {
            auto [edges, vertex_range] = graphs::get_rhg_explicit_num_edges(config.log_n, numEdges, 3.0, weights);
            distEdges = edges;
        } else if (config.graphType == "rgg2D") {
          //  auto [edges, vertex_range] = graphs::get_rgg2D(config.log_n, numEdges, weights);
          //  std::cout << edges.size() << std::endl;
          //  distEdges = edges;
        }


        if (config.shuffle) {
            distributedShuffle(distEdges);
        }


        return distEdges;
    }

    template<typename Edge>
    std::vector<Edge> getAllEdges(std::vector<Edge> &distEdges) {
        hybridMST::mpi::MPIContext ctx;
        return hybridMST::mpi::gatherv(distEdges.data(), distEdges.size(), 0, ctx);

    }


}