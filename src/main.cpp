#include "taow/cli_builder_macros.hpp"
#include <iostream>
#include <string>
#include <vector>

#define COMMAND_TEST_DEFINITION(X)                                                                                     \
    X(device, ARG, STRING)                                                                                             \
    X(test, ARG, STRING)

CLI_COMMAND(test_command, COMMAND_TEST_DEFINITION) {
    std::cout << "device is " << data.device << std::endl;
    return 0;
};

int main(int argc, char* argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};
    for (const auto& val : args) {
        std::cout << val << std::endl;
    }
    return 0;
}