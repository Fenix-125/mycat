#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <sys/stat.h> // struct for fstat() lstat() stat()
#include <fcntl.h> // types and constants for fcntl() and open()
#include <unistd.h> // POSIX header


#define BUFFER_SIZE 1'000'000

namespace po = boost::program_options;

void print_help(const po::options_description &options_description);

int main(int argc, char **argv) {
    //////////////////////////////////////////////// Local Vars ////////////////////////////////////////////////////////
    std::vector<std::string> file_list;
    bool hex_print;
    std::unique_ptr<char> buf(new char[BUFFER_SIZE]);
    //////////////////////////////////////////////// Local Vars END ////////////////////////////////////////////////////

    //////////////////////////////////////////////// Parse Options /////////////////////////////////////////////////////
    po::options_description visible("Supported options");
    visible.add_options()
            ("help,h", "Print this help message.")
            (",A", po::value<bool>(&hex_print)->zero_tokens(),
             "Print invisible symbols as hex codes (e.g. \\xAB) except whitespaces.");

    po::options_description hidden("Hidden options");
    hidden.add_options()
            ("file", po::value<std::vector<std::string>>(&file_list)->composing(), "File names to be processed.");

    po::positional_options_description p;
    p.add("file", -1);

    po::options_description all("All options");
    all.add(visible).add(hidden);

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(all).positional(p).run(), vm);
    } catch (boost::wrapexcept<po::unknown_option> &e) {
        std::cerr << "Error: unrecognised option '" << e.get_option_name() << "'" << std::endl;
        print_help(visible);
        return EXIT_FAILURE;
    }
    po::notify(vm);

    if (vm.count("help")) {
        print_help(visible);
        return EXIT_SUCCESS;
    }

    if (vm.count("file") == 0) {
        std::cerr << "Error: not enough file names given!" << std::endl;
        print_help(visible);
        return EXIT_FAILURE;
    }
    //////////////////////////////////////////////// Parse Options END /////////////////////////////////////////////////

    std::vector<int> file_descriptors(file_list.size());

    for (size_t f_index = 0; f_index < file_list.size(); ++f_index) {
        int f_descr = open(file_list[f_index].c_str(), O_RDONLY);
        if (f_descr == -1) {
            for (size_t i = 0; i < f_index; ++i) {
                if (close(file_descriptors[i]) == -1) {
                    std::cerr << "Error: can not close file! " << file_list[i] << std::endl;
                }
            }
            std::cerr << "Error: can not open file! " << file_list[f_index] << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (size_t f_index = 0; f_index < file_descriptors.size(); ++f_index) {

        size_t bytes_read = read(file_descriptors[f_index], buf.get(), BUFFER_SIZE);
        buf.get()[bytes_read] = 0;

        std::cout << "File name: " << file_list[f_index] << std::endl;
        std::cout << "Read count: " << bytes_read << "/200" << std::endl;
        std::cout << "Buf:\n" << buf.get() << std::endl;
    }

    int exit_code = EXIT_SUCCESS;
    for (size_t f_index = 0; f_index < file_descriptors.size(); ++f_index) {
        if (close(file_descriptors[f_index]) == -1) {
            exit_code = EXIT_FAILURE;
            std::cerr << "Error: can not close file! " << file_list[f_index] << std::endl;
        }
    }

    // Print input params
//    std::cout << "File List (" << file_list.size() << "):" << std::endl;
//    for (const auto& el : file_list) {
//        std::cout << "\t" << el << std::endl;
//    }
//    std::cout << "\nhex_print\t: " << (hex_print ? "true" : "false") << std::endl;
    return exit_code;
}

void print_help(const po::options_description &options_description) {
    std::cout << "Usage:\n  mycat [-h|--help] [-A] <file1> <file2> ... <fileN>\n" << options_description << std::endl;
}

int writebuffer(int fd, const char *buffer, ssize_t size, int *status) {
    ssize_t written_bytes = 0;
    while (written_bytes < size) {
        ssize_t written_now = write(fd, buffer + written_bytes,
                                    size - written_bytes);
        if (written_now == -1) {
            if (errno == EINTR)
                continue;
            else {
                *status = errno;
                return -1;
            }
        } else
            written_bytes += written_now;
    }
    return 0;
}

int readbuffer(int fd, const char *buffer, ssize_t size, int *status) { return 0; }
