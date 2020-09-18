//
// Created by fenix on 14.09.20.
//

#ifndef MYCAT_MYCAT_OPTS_H
#define MYCAT_MYCAT_OPTS_H

#include <string>
#include <exception>
#include <boost/program_options.hpp>


namespace ucu {
    namespace po = boost::program_options;

    class mycat_opts {
    public:
        mycat_opts();

        mycat_opts(int argc, char **argv);

        ~mycat_opts() = default;

        void parse(int argc, char **argv);

        [[nodiscard]] std::string get_help_msg() const;

        [[nodiscard]] bool get_hex_print_ind() const { return hex_print; }

        [[nodiscard]] const std::vector<std::string> &get_file_list() const { return file_list; }

    private:
        // declare all parameters
        std::vector<std::string> file_list{};
        bool hex_print = false;
        static const char usage_msg[];

        po::options_description visible{"Supported options"};
        po::options_description hidden{"Hidden options"};
        po::options_description all_opts{"All options"};
        po::positional_options_description positional{};
        po::variables_map var_map{};

        void init_opt_description();
    };

    class invalid_opts : public std::exception {
    public:
        [[nodiscard]] const char *what() const noexcept override {
            return "Error: Unrecognised command line option!";
        }
    };

    class zero_pos_opts : public std::exception {
    public:
        [[nodiscard]] const char *what() const noexcept override {
            return "Error: No files specified. Provide at least one file_name!";
        }
    };

    class help_option_condition : public std::exception {
    public:
        [[nodiscard]] const char *what() const noexcept override {
            return "help request";
        }
    };
}

#endif //MYCAT_MYCAT_OPTS_H
