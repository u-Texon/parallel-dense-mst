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
        Ams::sortLevel(mapper.get_mpi_datatype(), shuffleEdges, 2, gen, ctx.communicator(),
                       ShuffleOrder<std::pair<Edge, std::size_t>>{});
        edges.clear();
        for (auto &edge: shuffleEdges) {
            edges.push_back(edge.first);
        }
    }


    std::vector<WEdge>
    generatePairGraph(size_t log_n, size_t log_m, graphs::WeightGeneratorConfig<Weight> weightConfig) {
        hybridMST::mpi::MPIContext ctx;
        std::vector<WEdge> edges;
        auto n = (size_t) pow(2, (double) log_n);

        for (int weight = 1; weight < log_n + 1; ++weight) { //nur prozess 0
            size_t s = 0;
            size_t i = 0;
            while (i < n / (std::size_t) pow(2, weight)) {
                size_t t = s + (std::size_t) (pow(2, weight) / 2);
                WEdge newEdge(s, t, weight);
                WEdge reversedEdge(t, s, weight);
                edges.push_back(newEdge);
                edges.push_back(reversedEdge);
                s += (std::size_t) pow(2, weight);
                i++;
            }
        }

        weightConfig.min_weight = log_n + 2;
        auto [randomEdges, _] = graphs::get_gnm(log_n, log_m, weightConfig);
        randomEdges.erase(std::remove_if(randomEdges.begin(), randomEdges.end(), [&](const auto &edge) {
            size_t s = edge.src;
            size_t t = edge.dst;
            if (edge.src < edge.dst) {
                s = edge.dst;
                t = edge.src;
            }
            size_t dist = s - t;
            if (s % (dist * 2) == 0) { //s can only be one of those
                for (int weight = 1; weight < log_n + 1; ++weight) {
                    size_t power = pow(2, weight);
                    if (dist == power) {
                        //distance between s and t has to be 2^x
                        return true;
                    }
                }
            }
            return false;
            /*
            for (auto e: edges) {
                if (edge.src == e.src && e.dst == edge.dst) {
                    return true;
                }
            }
            return false;
             */
        }), randomEdges.end());


        auto rng = std::default_random_engine{};
        std::shuffle(edges.begin(), edges.end(), rng);
        int edgesPerProc = edges.size() / ctx.size();
        std::vector<WEdge> scatteredEdges;
        scatteredEdges.resize(edgesPerProc);
        hybridMST::mpi::TypeMapper<WEdge> mapper;

        MPI_Scatter(edges.data(), edgesPerProc, mapper.get_mpi_datatype(), scatteredEdges.data(), edgesPerProc,
                    mapper.get_mpi_datatype(), 0, ctx.communicator());


        scatteredEdges.insert(scatteredEdges.end(), randomEdges.begin(), randomEdges.end());
        return scatteredEdges;
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
            auto [edges, vertex_range] = graphs::get_rgg2D(config.log_n, static_cast<std::size_t>(numEdges), weights);
            distEdges = edges;
        } else if (config.graphType == "pair") {
            distEdges = generatePairGraph(config.log_n, config.log_m, weights);
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