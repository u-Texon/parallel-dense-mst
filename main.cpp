#include <iostream>
#include "include/util/timer.hpp"
#include "algorithms/kruskal.hpp"
#include "include/definitions.hpp"
#include "algorithms/filter_kruskal.hpp"
#include "algorithms/mergeMST.hpp"
#include "external/graphs/interface.hpp"
#include "algorithms/dense_boruvka.hpp"
#include "include/mpi/gather.hpp"

#define LOG_M 18
#define LOG_N 10
#define MAX_W 1000000


std::pair<WEdgeList, VId> testMergeMST(int &n, WEdgeList &edges, hybridMST::Timer &timer) {
    timer.start("merge", 0);
    WEdgeList mergeMst = mergeMST::getMST(n, edges);
    timer.stop("merge", 0);
    VId mergeWeight = 0;
    for (auto &edge: mergeMst) {
        mergeWeight += edge.get_weight();
    }
    std::pair<WEdgeList, VId> pair;
    pair.first = mergeMst;
    pair.second = mergeWeight;
    return pair;
}


std::pair<WEdgeList, VId> testKruskal(int &n, WEdgeList &edges, hybridMST::Timer &timer) {
    timer.start("kruskal", 0);
    UnionFind uf(n);
    WEdgeList mst = kruskal::getMST(edges, uf);
    timer.stop("kruskal", 0);
    VId kruskalWeight = 0;
    for (auto &edge: mst) {

        kruskalWeight += edge.get_weight();
    }
    std::pair<WEdgeList, VId> pair(mst, kruskalWeight);
    return pair;
}

std::pair<WEdgeList, VId> testFilterKruskal(int &n, WEdgeList &edges, hybridMST::Timer &timer) {
    timer.start("filter", 0);
    UnionFind uf(n);
    std::vector<WEdge> mst = filterKruskal::getMST(n, edges, uf);
    timer.stop("filter", 0);

    VId filterWeight = 0;
    for (auto &edge: mst) {
        filterWeight += edge.get_weight();
    }

    std::pair<WEdgeList, VId> pair(mst, filterWeight);
    return pair;
}


std::pair<WEdgeList, VId> testDenseBoruvka(int n, WEdgeList &edges, hybridMST::Timer &timer) {

    WEdgeOriginList newEdges;
    for (auto &edge: edges) {
        newEdges.push_back(WEdgeOrigin(edge.get_src(), edge.get_dst(), edge.get_weight()));
    }

    timer.start("denseBoruvka", 0);
    std::vector<WEdge> mst = dense_boruvka::getMST(n, newEdges);
    timer.stop("denseBoruvka", 0);

    VId bWeight = 0;
    for (auto &edge: mst) {
        bWeight += edge.get_weight();
    }
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
        std::cout << "calculations start!" << std::endl;
    }

    hybridMST::Timer timer;
    auto [denseBoruvkaMST, bWeight] = testDenseBoruvka(n, distEdges, timer);
    auto [mergeMST, mergeWeight] = testMergeMST(n, distEdges, timer);
    auto [kruskalMST, kruskalWeight] = testKruskal(n, allEdges, timer);
    auto [filterMST, filterWeight] = testFilterKruskal(n, allEdges, timer);









      if (ctx.rank() == 0) { //local tests

          if (kruskalWeight != bWeight) {
              std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
              std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
              std::cout << "dense boruvka found MST with weight: " << bWeight << std::endl;
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
