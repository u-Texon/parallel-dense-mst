#pragma once

#include <string>
#include "../executions/executor.hpp"
#include "../include/definitions.hpp"

namespace commandParser {


    void parseArgument(const std::string& arg, Config &c) {
        hybridMST::mpi::MPIContext ctx;

        if (arg == "help") {
            //TODO: add help documentation
            if (ctx.rank() == 0) {
                std::cout << "help info" << std::endl;
            }
        } else if (arg == "t") {
            c.test = true;
        } else if (arg.substr(0, 2) == "n=") {
            c.log_n = std::stoi(arg.substr(2, arg.length()));
        } else if (arg.substr(0, 2) == "m=") {
            c.log_m = std::stoi(arg.substr(2, arg.length()));
        } else if (arg.substr(0, 2) == "g=") {
            c.graphType = arg.substr(2, arg.length());
        } else {
            if (ctx.rank() == 0) {
                std::cout << "command " << arg << " not found" << std::endl;
            }
        }
    }





    void parseCommand(int &size, char *args[]) {
        hybridMST::mpi::MPIContext ctx;

        Config config;
        if (size <= 1) {
            if (ctx.rank() == 0) {
                std::cout << "No arguments were provided. See <help> for help" << std::endl;
                std::cout << "Program will run with default configurations" << std::endl;
            }
        } else {
            for (int i = 1; i < size; ++i) {
                parseArgument(args[i], config);
            }
        }

        if (ctx.rank() == 0) {
            std::cout << "starting algorithm on " << config << std::endl;
        }

        executor::executeCommand(config);
    }

}


