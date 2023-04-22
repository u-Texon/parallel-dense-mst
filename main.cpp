#include <iostream>
#include "include/util/timer.hpp"
#include "generateGraphs/generateGraph.hpp"
#include "algorithms/kruskal.hpp"
#include "include/definitions.hpp"
#include "algorithms/filter_kruskal.hpp"

int main() {
        std::cout << "Program started" << std::endl;
    hybridMST::Timer timer;
    timer.start("timer", 0);
        std::cout << "Timer started" << std::endl;

    //int solution = 23490234 * 247238423894789;
    timer.stop("timer", 0);

        std::cout << "Timer stopped with output: " << timer.output()  << std::endl;


    int edgeCount = 100;
    int vertexCount = 20;
    int maxWeight = 40;


    std::vector<WEdge> edges = generate::randomGraph(edgeCount, vertexCount, maxWeight);

    std::cout << "edges are :" << std::endl;
    for (auto & edge : edges)
    {

        std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
    }

    std::cout << std::endl;

    UnionFind uf(vertexCount);
    std::vector<WEdge> mst = kruskal::getMST(edges, &uf);
    std::cout << "MST-edges are :" << std::endl;
    int kruskalWeight = 0;
    for (auto & edge: mst) {
        std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        kruskalWeight += edge.get_weight();
    }
    std::cout << std::endl;


    UnionFind uf2(vertexCount);
    std::vector<WEdge> mst2 = filterKruskal::getMST(edges, &uf2);
    int filterWeight = 0;
    for (auto & edge: mst2) {
        std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
        filterWeight += edge.get_weight();
    }
    std::cout << std::endl;

    std::cout << "kruskal found MST with weight: " << kruskalWeight << std::endl;
    std::cout << "filter-kruskal found MST with weight: " << filterWeight << std::endl;

    return 0;
}
