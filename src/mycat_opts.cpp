//
// Created by fenix on 14.09.20.
//

#include <mycat_opts.h>

ucu::mycat_opts::mycat_opts() {
    init_opt_description();
}

ucu::mycat_opts::mycat_opts(const int argc, char **argv) {
    init_opt_description();
    parse(argc, argv);
}

void ucu::mycat_opts::init_opt_description() {
    visible.add_options()
            ("help,h", "Print this help message.")
            (",A", po::value<bool>(&hex_print)->zero_tokens(),
             "Print invisible symbols as hex codes (e.g. \\xAB) except whitespaces.");

    hidden.add_options()
            ("file", po::value<std::vector<std::string>>(&file_list)->composing(),
             "File names to be processed.");

    positional.add("file", -1);
    all_opts.add(visible).add(hidden);
}

void ucu::mycat_opts::parse(const int argc, char **argv) {
    try {
        po::store(po::command_line_parser(argc, argv).options(all_opts).positional(positional).run(),
                  var_map);
        var_map.notify();
    } catch (boost::wrapexcept<po::unknown_option> &e) {
        throw invalid_opts{};
    }
    if (var_map.count("help")) {
        throw help_option_condition{};
    } else if (var_map.count("file") == 0) {
        throw zero_pos_opts{};
    }
}

const char ucu::mycat_opts::usage_msg[] = "Usage:\n  mycat [-h|--help] [-A] <file1> <file2> ... <fileN>\n";

std::string ucu::mycat_opts::get_help_msg() const {
    std::stringstream s{};
    s << usage_msg << visible;
    return s.str();
}
