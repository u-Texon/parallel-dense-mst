#include <iostream>
#include "include/util/timer.hpp"
#include "algorithms/kruskal.hpp"
#include "include/definitions.hpp"
#include "algorithms/filter_kruskal.hpp"
#include "algorithms/mergeMST.hpp"
#include "external/graphs/interface.hpp"
#include "algorithms/dense_boruvka.hpp"
#include "include/mpi/gather.hpp"
#include <mpi.h>

#define LOG_M 3
#define LOG_N 3
#define MAX_W 1000000


std::pair<WEdgeList, VId> testMergeMST(int *n, WEdgeList *edges, hybridMST::Timer *timer) {
    timer->start("merge", 0);
    WEdgeList mergeMst = mergeMST::getMST(n, edges);
    timer->stop("merge", 0);
    // std::cout << "MergeMST-edges are :" << std::endl;
    VId mergeWeight = 0;
    for (auto edge: mergeMst) {
        //   std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        mergeWeight += edge.get_weight();
    }
    //std::cout << std::endl;

    std::pair<WEdgeList, VId> pair;
    pair.first = mergeMst;
    pair.second = mergeWeight;
    return pair;
}


std::pair<WEdgeList, VId> testKruskal(int *n, WEdgeList *edges, hybridMST::Timer *timer) {
    timer->start("kruskal", 0);
    UnionFind uf(*n);
    WEdgeList mst = kruskal::getMST(edges, &uf);
    timer->stop("kruskal", 0);

    // std::cout << "MST-edges are :" << std::endl;
    VId kruskalWeight = 0;
    for (auto &edge: mst) {
        //  std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        kruskalWeight += edge.get_weight();
    }
    //std::cout << std::endl;
    std::pair<WEdgeList, VId> pair(mst, kruskalWeight);
    return pair;
}

std::pair<WEdgeList, VId> testFilterKruskal(int *n, WEdgeList *edges, hybridMST::Timer *timer) {

    UnionFind uf2(*n);
    timer->start("filter", 0);
    std::vector<WEdge> mst = filterKruskal::getMST(n, edges, &uf2);
    timer->stop("filter", 0);

    VId filterWeight = 0;
    for (auto &edge: mst) {
        //std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        filterWeight += edge.get_weight();
    }
    //std::cout << std::endl;

    std::pair<WEdgeList, VId> pair(mst, filterWeight);
    return pair;
}


std::pair<WEdgeList, VId> testDenseBoruvka(int *n, WEdgeList *edges, hybridMST::Timer *timer) {

    std::cout << "boruvka started" << std::endl;

    timer->start("denseBoruvka", 0);
    std::vector<WEdge> mst = dense_boruvka::getMST(n, edges);
    timer->stop("denseBoruvka", 0);

    VId bWeight = 0;
    for (auto &edge: mst) {
        //std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        bWeight += edge.get_weight();
    }
    //std::cout << std::endl;

    std::pair<WEdgeList, VId> pair(mst, bWeight);
    return pair;
}


int main() {
    hybridMST::mpi::MPIContext ctx;
    hybridMST::mpi::TypeMapper<WEdge> mapper;
    //generate graph
    auto [distEdges, vertex_range] = graphs::get_gnm(LOG_N, LOG_M);
    WEdgeList allEdges = hybridMST::mpi::gatherv(distEdges.data(), distEdges.size(), 0, ctx);




    int n = (int) pow(2, LOG_N);


    if (ctx.rank() == 0) {
        //std::cout << "edgecount: " << distEdges.size() << std::endl;
        //std::cout << "vertexcount: " << n << " or " << distEdges.at(distEdges.size() - 1).get_src() << std::endl;
        std::cout << "calculations start!" << std::endl;
    }

    hybridMST::Timer timer;
    auto [mergeMST, mergeWeight] = testMergeMST(&n, &distEdges, &timer);
    auto [kruskalMST, kruskalWeight] = testKruskal(&n, &allEdges, &timer);
    auto [filterMST, filterWeight] = testFilterKruskal(&n, &allEdges, &timer);
    auto [denseBoruvkaMST, bWeight] = testDenseBoruvka(&n, &allEdges, &timer);



      if (ctx.rank() == 0) { //local tests
          if (kruskalWeight != bWeight) {
              std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
              std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
              std::cout << "dense boruvka found MST with weight: " << filterWeight << std::endl;
              std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
          }

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
