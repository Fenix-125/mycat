#include <iostream>
#include <string>
#include <vector>

#include <mycat_opts.h>
#include <mycat.h>
#include <mycat_hex.h>


int main(int argc, char **argv) {
    ucu::mycat_opts opts{};

    try {
        opts.parse(argc, argv);
    } catch (const ucu::help_option_condition &e) {
        std::cout << opts.get_help_msg() << std::endl;
        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        std::cout << opts.get_help_msg() << std::endl;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    try {
        if (opts.get_hex_print_ind()) {
            ucu::mycat_hex{opts}.run();
        } else {
            ucu::mycat{opts}.run();
        }
    } catch (const ucu::mycat_error &e) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


