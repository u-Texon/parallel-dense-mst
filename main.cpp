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


    generate::Generator g;
    g.randomGraph(18, 8);

    return 0;
}
