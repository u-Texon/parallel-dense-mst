#include <iostream>
#include "include/util/timer.hpp"
#include "generateGraphs/generateGraph.hpp"
#include "algorithms/kruskal.hpp"
#include "include/definitions.hpp"

int main() {
        std::cout << "Program started" << std::endl;
    hybridMST::Timer timer;
    timer.start("timer", 0);
        std::cout << "Timer started" << std::endl;

    //int solution = 23490234 * 247238423894789;
    timer.stop("timer", 0);

        std::cout << "Timer stopped with output: " << timer.output()  << std::endl;


    int edgeCount = 6;
    int vertexCount = 4;
    int maxWeight = 10;


    std::vector<WEdge> edges = generate::randomGraph(edgeCount, vertexCount, maxWeight);

    std::cout << "edges are :" << std::endl;
    for (auto & edge : edges)
    {

        std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
    }

    std::cout << std::endl;

    std::vector<WEdge> mst = kruskal::getMST(vertexCount, edges);
    std::cout << "MST-edges are :" << std::endl;
    for (auto & edge: mst) {
        std::cout << "(" << edge.get_src() << "," << edge.get_dst() << "," << edge.get_weight() << ") ";
    }

    std::cout << std::endl;

    return 0;
}
