#pragma once

#include <string>
#include "../executions/executor.hpp"
#include "../include/definitions.hpp"

namespace commandParser {


    void printHelpInfo() {
        //TODO: complete
        std::cout
                << "You can change the number of edges and vertices with the parameters \"m=<number>\" and \"n=<number>\""
                << std::endl;
        std::cout << "Keep in mind that the amount of vertices/edges is 2^<your number>" << std::endl;
        std::cout << std::endl;
        std::cout << "Tests will run if the parameter \"t\" is provided" << std::endl;
        std::cout << std::endl;
        std::cout << "You can select the min/maxWeight with \"min=<type>\" and \"max=<type>\"" << std::endl;
        std::cout << "You can select the graphtype with \"g=<type>\"" << std::endl;
        std::cout << "Possible Graph types are: rhg,gnm, ..." << std::endl;

        std::cout << "You can select an algorithm with \"algo=<algorithm>\"" << std::endl;

        std::cout << std::endl;
        std::cout << "If you do not provide all arguments, the remaining configuration will start with default values"
                  << std::endl;
    }


    void parseArgument(const std::string &arg, Config &config) {
        hybridMST::mpi::MPIContext ctx;

        if (arg == "help" || arg == "h" || arg == "-h") {
            config.help = true;
        } else if (arg == "t") {
            config.test = true;
        } else if (arg.substr(0, 2) == "n=") {
            config.log_n = std::stoi(arg.substr(2, arg.length()));
        } else if (arg.substr(0, 2) == "m=") {
            config.log_m = std::stoi(arg.substr(2, arg.length()));
        } else if (arg.substr(0, 4) == "min=") {
            config.minWeight = std::stoi(arg.substr(4, arg.length()));
        } else if (arg.substr(0, 4) == "max=") {
            config.maxWeight = std::stoi(arg.substr(4, arg.length()));
        } else if (arg.substr(0, 2) == "g=") {
            config.graphType = arg.substr(2, arg.length());
        } else if (arg.substr(0, 5) == "algo=") {
            config.algo = arg.substr(5, arg.length());
        } else {
            if (ctx.rank() == 0) {
                std::cout << "command " << arg << " not found" << std::endl;
                std::cout << "Use argument <help> for help" << std::endl;
            }
            config.parseError = true;
        }
    }


    void parseCommand(int &size, char *args[]) {
        hybridMST::mpi::MPIContext ctx;

        Config config;
        if (size <= 1) {
            if (ctx.rank() == 0) {
                std::cout << "No arguments were provided. Use argument <help> for help" << std::endl;
                std::cout << "Program will run with default configurations" << std::endl;
            }
        } else {
            for (int i = 1; i < size; ++i) {
                try {
                    parseArgument(args[i], config);
                } catch (const std::exception &e) {
                    config.parseError = true;
                    if (ctx.rank() == 0) {
                        std::cout << "error on parsing " << args[i] << std::endl;
                    }
                }
            }
        }


        if (config.parseError) {
            return;
        } else if (config.help) {
            if (ctx.rank() == 0) {
                printHelpInfo();
            }
        } else {
            if (ctx.rank() == 0) {
                std::cout << "starting algorithm on " << config << std::endl;
            }
        }
        executor::executeCommand(config);
    }
}


