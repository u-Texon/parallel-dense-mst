#include <iostream>
#include "include/util/timer.hpp"
#include "generateGraphs/generateGraph.hpp"
#include "algorithms/kruskal.hpp"
#include "include/definitions.hpp"
#include "algorithms/filter_kruskal.hpp"
#include "algorithms/mergeMST.hpp"


#define M 400000
#define N 1000
#define MAX_W 1000000


int main() {
    WEdgeList edges = generate::randomGraph(M, N, MAX_W);
    int n = N;
    int m = M;

    hybridMST::mpi::MPIContext ctx;
    hybridMST::Timer timer;

    if (ctx.rank() == 0) {
        std::cout << "calculations start!" << std::endl;
    }

    timer.start("merge", 0);
    WEdgeList mergeMst = mergeMST::getMST(&n, &m, edges);
    timer.stop("merge", 0);


    timer.start("kruskal", 0);
    UnionFind uf(N);
    WEdgeList mst = kruskal::getMST(&edges, &uf);
    timer.stop("kruskal", 0);

    // std::cout << "MST-edges are :" << std::endl;
    VId kruskalWeight = 0;
    for (auto &edge: mst) {
        //  std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        kruskalWeight += edge.get_weight();
    }
    //std::cout << std::endl;


    // std::cout << "MergeMST-edges are :" << std::endl;
    int mergeWeight = 0;
    for (auto edge: mergeMst) {
        //   std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        mergeWeight += edge.get_weight();
    }
    //std::cout << std::endl;


    UnionFind uf2(N);
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


    return 0;
}
