#include <iostream>
#include "include/util/timer.hpp"
#include "generateGraphs/generateGraph.hpp"

int main() {
        std::cout << "Programm started" << std::endl;
    hybridMST::Timer timer;
    timer.start("timer", 0);
        std::cout << "Timer started" << std::endl;

    //int solution = 23490234 * 247238423894789;
    timer.stop("timer", 0);

        std::cout << "Timer stopped with output: " << timer.output()  << std::endl;


    int edgeCount = 5;
    int vertexCount = 7;

    generate::Generator g;
    std::vector<generate::Edge> edges = g.randomGraph(edgeCount, vertexCount);

    for (size_t i = 0; i < edges.size(); i++)
    {
        std::cout << "(" << edges[i].source << "," << edges[i].target <<  ")" << std::endl;
    }
    

    return 0;
}
