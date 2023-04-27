#include <iostream>
#include "include/util/timer.hpp"
#include "generateGraphs/generateGraph.hpp"
#include "algorithms/kruskal.hpp"
#include "include/definitions.hpp"
#include "algorithms/filter_kruskal.hpp"
#include "algorithms/mergeMST.hpp"


#define M 803
#define N 606
#define MAX_W 1400


int main(int argc, char **args) {
    WEdgeList edges = generate::randomGraph(M, N, MAX_W);
    int n = N;
    int m = M;


    WEdgeList mergeMst =  mergeMST::getMST(&n, &m, edges);

    hybridMST::mpi::MPIContext ctx;
    int rank = ctx.rank();


    if (rank == 0) {

        UnionFind uf(N);
        hybridMST::Timer timer;
        //timer.start("timer", 0);

        std::vector<WEdge> mst = kruskal::getMST(&edges, &uf);
        //  timer.stop("timer", 0);
        // std::cout << "Kruskal stopped with output: " << timer.output() << std::endl;
        // timer.reset();

       // std::cout << "MST-edges are :" << std::endl;
        int kruskalWeight = 0;
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

        std::cout << "kruskal got mst with weight " << kruskalWeight << std::endl;
        std::cout << "mergeMST got mst with weight " << mergeWeight << std::endl;

        /*
        UnionFind uf2(N);
        //  timer.start("timer", 0);
        std::vector<WEdge> mst2 = filterKruskal::getMST(&n, &edges, &uf2);
        //  timer.stop("timer", 0);
        //  std::cout << "Filter Kruskal stopped with output: " << timer.output() << std::endl;

        int filterWeight = 0;
        for (auto &edge: mst2) {
            //std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
            filterWeight += edge.get_weight();
        }
        std::cout << std::endl;


        if (kruskalWeight != filterWeight) {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
            std::cout << "filter-kruskal found MST with weight: " << filterWeight << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }


    }

    */
    }
    return 0;
}
