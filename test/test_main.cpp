#define CATCH_CONFIG_CONSOLE_WIDTH 300
#define CATCH_CONFIG_EXTERNAL_INTERFACES
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"
#include <stdint.h>
#include <iostream>

int main(int argc, char* argv[])
{
    Catch::Session session;

    // Build a new parser on top of Catch2's
    using namespace Catch::clara;
    // Let Catch2 (using Clara) parse the command line
    int return_code = session.applyCommandLine(argc, argv);
    if (return_code != 0) // Indicates a command line error
        return return_code;

    int result = session.run(argc, argv);

    return result;
}
