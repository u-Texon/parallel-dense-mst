#include <iostream>
#include "include/util/timer.hpp"
#include "generateGraphs/generateGraph.hpp"
#include "algorithms/kruskal.hpp"
#include "include/definitions.hpp"
#include "algorithms/filter_kruskal.hpp"
#include "algorithms/mergeMST.hpp"
#include "external/graphs/interface.hpp"

#define LOG_M 15
#define LOG_N 10
#define MAX_W 1000000


int main() {
    hybridMST::mpi::MPIContext ctx;
    auto [edges, vertex_range] = graphs::get_gnm(LOG_N, LOG_M);

    int n = (int) pow(2, LOG_N);

    hybridMST::Timer timer;

    if (ctx.rank() == 0) {
        std::cout << "edgecount: " << edges.size() << std::endl;
        std::cout << "vertexcount: " << n << " or " << edges.at(edges.size() -1).get_src() << std::endl;
        std::cout << "calculations start!" << std::endl;
    }


    timer.start("merge", 0);
    WEdgeList mergeMst = mergeMST::getMST(&n, edges);
    timer.stop("merge", 0);
    // std::cout << "MergeMST-edges are :" << std::endl;
    int mergeWeight = 0;
    for (auto edge: mergeMst) {
        //   std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        mergeWeight += edge.get_weight();
    }
    //std::cout << std::endl;


    timer.start("kruskal", 0);
    UnionFind uf(n);
    WEdgeList mst = kruskal::getMST(&edges, &uf);
    timer.stop("kruskal", 0);

    // std::cout << "MST-edges are :" << std::endl;
    VId kruskalWeight = 0;
    for (auto &edge: mst) {
        //  std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        kruskalWeight += edge.get_weight();
    }
    //std::cout << std::endl;





    UnionFind uf2(n);
    timer.start("filter", 0);
    std::vector<WEdge> mst2 = filterKruskal::getMST(&n, &edges, &uf2);
    timer.stop("filter", 0);

    VId filterWeight = 0;
    for (auto &edge: mst2) {
        //std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        filterWeight += edge.get_weight();
    }
    //std::cout << std::endl;


    if (ctx.rank() == 0) {
        if (kruskalWeight != filterWeight) {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
            std::cout << "filter-kruskal found MST with weight: " << filterWeight << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }
        if (kruskalWeight != mergeWeight) {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
            std::cout << "mergeMST found MST with weight: " << mergeWeight << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }

        std::cout << "calculations complete!" << std::endl;
    }


    std::cout << timer.output();
    if (ctx.rank() == 0) {
        std::cout << std::endl;
    }


    hybridMST::mpi::MPIContext::finalize();
    return 0;
}
