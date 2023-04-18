#include <iostream>
#include "include/util/timer.hpp"

int main() {
        std::cout << "Programm started" << std::endl;
    hybridMST::get_timer().start("timer", 0);
        std::cout << "Timer started" << std::endl;

    //int solution = 23490234 * 247238423894789;
    hybridMST::get_timer().stop("timer", 0);
        std::cout << "Timer stopped" << std::endl;
    return 0;
}
