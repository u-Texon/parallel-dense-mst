#pragma once

#include "../external/graphs/interface.hpp"
#include "../include/mpi/gather.hpp"

#include "AmsSort/AmsSort.hpp"


namespace generateGraph {

    //TODO: shuffle edges
    //kanten random ids geben und dann:
    //ams sort zum verteilen



    template<typename EdgeType>
    struct ShuffleOrder {
        bool operator()(const EdgeType &left, const EdgeType &right) const {
            return left.randomID < right.randomID;
        }
    };

    template<typename Edge, typename Compare = ShuffleOrder<Edge>>
    void distributedShuffle(std::vector<Edge> edges) {
        hybridMST::mpi::MPIContext ctx;

        std::vector<std::pair<Edge, std::size_t>> shuffleEdges;
        for (auto &edge: edges) {
            shuffleEdges.push_back({edge, rand() % (ctx.size())});
        }
        hybridMST::mpi::TypeMapper<std::pair<Edge, std::size_t>> mapper;
        std::mt19937_64 gen(ctx.rank());

      //  Ams::sortLevel(mapper.get_mpi_datatype(), shuffleEdges, 2, gen, ctx.communicator(), Compare{});

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

        if (config.graphType == "rhg") {
            auto [edges, vertex_range] = graphs::get_rhg(config.log_n, config.log_m, 3.0, weights);
            distEdges = edges;
        } else if (config.graphType == "gnm") {
            auto [edges, vertex_range] = graphs::get_gnm(config.log_n, config.log_m, weights);
            distEdges = edges;
        } else if (config.graphType == "rhgNumEdges") {
            auto [edges, vertex_range] = graphs::get_rhg_explicit_num_edges(config.log_n, config.log_m, 3.0, weights);
            distEdges = edges;
        }


        if (config.shuffle) {
            distributedShuffle(distEdges);
        }

        /*
        ctx.barrier();
        if (ctx.rank() == 0) {
            for (auto &edge: distEdges) {
                std::cout << "process " << ctx.rank() << " has edge " << edge << std::endl;
            }
        }

        ctx.barrier();
        if (ctx.rank() == 1) {
            for (auto &edge: distEdges) {
                std::cout << "process " << ctx.rank() << " has edge " << edge << std::endl;
            }
        }

        ctx.barrier();
        if (ctx.rank() == 2) {
            for (auto &edge: distEdges) {
                std::cout << "process " << ctx.rank() << " has edge " << edge << std::endl;
            }
        }

        ctx.barrier();
        if (ctx.rank() == 3) {
            for (auto &edge: distEdges) {
                std::cout << "process " << ctx.rank() << " has edge " << edge << std::endl;
            }
        }
        */

        return distEdges;
    }

    template<typename Edge>
    std::vector<Edge> getAllEdges(std::vector<Edge> &distEdges) {
        hybridMST::mpi::MPIContext ctx;
        return hybridMST::mpi::gatherv(distEdges.data(), distEdges.size(), 0, ctx);

    }


}