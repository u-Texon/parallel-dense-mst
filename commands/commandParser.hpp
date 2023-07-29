#pragma once

#include <string>
#include "../executions/executor.hpp"
#include "../include/definitions.hpp"

namespace commandParser {

    static std::vector<std::string> algorithms = {"boruvka", "kruskal", "filter", "mixedMerge", "merge",
                                                  "boruvkaMerge", "all", "allParallel"};
    static std::vector<std::string> graphTypes = {"rhg", "gnm", "rhgNumEdges", "rgg2D"};

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
        std::cout << "Possible Graph types are: ";
        for (auto &type: graphTypes) {
            std::cout << type << ", ";
        }
        std::cout << std::endl;

        std::cout << "You can select an algorithm with \"algo=<algorithm>\"" << std::endl;
        std::cout << "Possible algorithms are: ";
        for (auto &algo: algorithms) {
            std::cout << algo << ", ";
        }
        std::cout << std::endl;

        std::cout << std::endl;
        std::cout << "If you do not provide all arguments, the remaining configuration will start with default values"
                  << std::endl;
    }

    void checkGraphType(const std::string &name) {
        for (auto &type: graphTypes) {
            if (name == type) {
                return;
            }
        }
        throw std::invalid_argument("unknown graph-type");
    }

    void checkAlgo(const std::string &name) {
        for (auto &algo: algorithms) {
            if (name == algo) {
                return;
            }
        }
        throw std::invalid_argument("unknown algorithm");
    }

    void parseArgument(const std::string &arg, Config &config) {
        hybridMST::mpi::MPIContext ctx;

        if (arg == "help" || arg == "h" || arg == "-h") {
            config.help = true;
        } else if (arg == "k") {
            config.useKruskal = true;
        } else if (arg == "b") {
            config.boxplot = true;
        } else if (arg == "s") {
            config.shuffle = true;
        } else if (arg == "thread") {
            config.boruvkaThread = true;
        } else if (arg == "t") {
            config.test = true;
        } else if (arg == "o") {
            config.onlyThisAlgo = true;
        } else if (arg.substr(0, 2) == "p=") {
            config.edgesPerProc = std::stoi(arg.substr(2, arg.length()));
        } else if (arg.substr(0, 2) == "l=") {
            config.localMSTcount = std::stoi(arg.substr(2, arg.length()));
        } else if (arg.substr(0, 2) == "r=") {
            config.repeat = 1 + std::stoi(arg.substr(2, arg.length()));
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
            checkGraphType(config.graphType);
        } else if (arg.substr(0, 5) == "algo=") {
            config.algo = arg.substr(5, arg.length());
            checkAlgo(config.algo);
        } else if (arg.substr(0, 2) == "d=") {
            VId d = std::stoi(arg.substr(2, arg.length()));
            if (d < 2) {
                throw std::invalid_argument("tree factor needs to be >= 2");
            }
            config.treeFactor = d;
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
                std::cout << "No arguments were provided. Use argument <help> for help. ";
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
                        std::cout << e.what() << std::endl;
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
            return;
        } else {
            if (ctx.rank() == 0) {
                std::cout << "starting algorithm on " << config << ". number of processors: " << ctx.size()
                          << std::endl;
            }
        }
        executor::executeCommand(config);
    }
}


