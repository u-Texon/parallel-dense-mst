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
        if (ctx.rank() == 0) {
            for (int weight = 1; weight < log_n + 1; ++weight) {
                size_t s = 0;
                size_t i = 0;
                while (i < n / (std::size_t) pow(2, weight)) {
                    size_t t = s + (std::size_t) (pow(2, weight) / 2);
                    WEdge newEdge(s, t, weight);
                    WEdge reversedEdge(t, s, weight);
                    // std::cout << newEdge << std::endl;
                    edges.push_back(newEdge);
                    edges.push_back(reversedEdge);
                    s += (std::size_t) pow(2, weight);
                    i++;
                }
            }
            //add one more edge so that the amount equals n
            WEdge newEdge(0, 2, log_n + 2);
            WEdge reversedEdge(2, 0, log_n + 2);
            edges.push_back(newEdge);
            edges.push_back(reversedEdge);

            auto rng = std::default_random_engine{};
            std::shuffle(edges.begin(), edges.end(), rng);
            std::cout << "edgecount: " << edges.size() << std::endl;
        }

        int edgesPerProc = n / ctx.size();
        std::vector<WEdge> scatteredEdges;
        scatteredEdges.resize(edgesPerProc);
        hybridMST::mpi::TypeMapper<WEdge> mapper;

        MPI_Scatter(edges.data(), edgesPerProc, mapper.get_mpi_datatype(), scatteredEdges.data(), edgesPerProc, mapper.get_mpi_datatype(), 0, ctx.communicator());
        /*
        ctx.barrier();
        for (int i = 0; i < ctx.size(); ++i) {
            if (ctx.rank() == i) {
                for(auto e: scatteredEdges) {
                    std::cout << ctx.rank() << " has " << e << std::endl;
                }
            }
            ctx.barrier();
        }*/


        //TODO: generate some more edges to complete the graph
        weightConfig.min_weight = log_n + 3;


        return edges;
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