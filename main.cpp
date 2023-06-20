#include "commands/commandParser.hpp"


int main(int argc, char *argv[]) {
    hybridMST::mpi::MPIContext::initialize();
    commandParser::parseCommand(argc, argv); //parses commands and executes them
    hybridMST::mpi::MPIContext::finalize();
    return 0;
}
