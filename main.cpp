#include "commands/commandParser.hpp"


int main(int argc, char *argv[]) {
    commandParser::parseCommand(argc, argv); //parses commands and executes them
    return 0;
}
