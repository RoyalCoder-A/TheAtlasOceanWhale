#include "taow/cli_builder_macros.hpp"
#include <iostream>
#include <string>
#include <vector>

#define TEST_COMMAND_OPTIONS(X)                                                                                        \
    X(device, STRING)                                                                                                  \
    X(test, INT)

#define TEST_COMMAND_ARGS(X) X(name, 0, STRING)

CLI_COMMAND(test_command, TEST_COMMAND_OPTIONS, TEST_COMMAND_ARGS) {
    std::cout << "device is " << (options.device ? options.device.value() : "NA") << std::endl;
    return 0;
};

int main(int argc, char* argv[]) {
    std::vector<std::string> args{argv + 1, argv + argc};
    for (const auto& val : args) {
        std::cout << val << std::endl;
    }
    return 0;
}