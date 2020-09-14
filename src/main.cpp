#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <sys/stat.h> // struct for fstat() lstat() stat()
#include <fcntl.h> // types and constants for fcntl() and open()
#include <unistd.h> // POSIX header


#define BUFFER_SIZE     1'000'000
//#define BUFFER_SIZE     100
#define HEX_STR_SIZE    4

#define SUCCESS_CODE    0
#define FAIL_CODE       -1

#define STD_OUT_DECR    1
#define STD_ERR_DECR    2

namespace po = boost::program_options;

void print_help(const po::options_description &options_description);

ssize_t read_buffer(int fd, char *buffer, ssize_t size, int *status);

int write_buffer(int fd, const char *buffer, ssize_t size, int *status);

unsigned char num_to_hex_char(unsigned char n);

void write_as_hex(char *buf, unsigned char c);

int main(int argc, char **argv) {
    //////////////////////////////////////////////// Local Vars ////////////////////////////////////////////////////////
    std::vector<std::string> file_list;
    bool hex_print;
    std::unique_ptr<char> read_buf(new char[BUFFER_SIZE]);
    std::unique_ptr<char> write_buf;
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
        file_descriptors[f_index] = open(file_list[f_index].c_str(), O_RDONLY);
        if (file_descriptors[f_index] == FAIL_CODE) {
            for (size_t i = 0; i < f_index; ++i) {
                if (close(file_descriptors[i]) == -1) {
                    std::cerr << "Error: can not close file! " << file_list[i] << std::endl;
                }
            }
            std::cerr << "Error: can not open file! " << file_list[f_index] << std::endl;
            return EXIT_FAILURE;
        }
    }


    int status = 0;
    ssize_t read_bytes;
    bool no_io_error = true;
    int exit_code = EXIT_SUCCESS;
    ssize_t wr_buf_index, rd_buf_index;
    if (hex_print) {
        write_buf.reset(new char[BUFFER_SIZE]);
    } else {
        write_buf.reset(read_buf.get()); // TODO: consider change as they are unique pointers
    }
    for (size_t f_index = 0; no_io_error && f_index < file_descriptors.size(); ++f_index) {
        do {
            read_bytes = read_buffer(file_descriptors[f_index], read_buf.get(), BUFFER_SIZE, &status);
            if (read_bytes == FAIL_CODE) {
                std::cerr << "Error: while read form file: " << file_list[f_index] << std::endl;
                no_io_error = false;
                exit_code = EXIT_FAILURE;
                break;
            }

            if (hex_print) {
                wr_buf_index = rd_buf_index = 0;
                for (rd_buf_index = 0; rd_buf_index < read_bytes ; ++rd_buf_index) {
                    const auto c = read_buf.get()[rd_buf_index];
                    if (!isspace(static_cast<int>(c)) && !isprint(static_cast<int>(c))) {
                        write_as_hex(&write_buf.get()[wr_buf_index], c);
                        wr_buf_index += HEX_STR_SIZE;
                    } else {
                        write_buf.get()[wr_buf_index] = c;
                    }
                }
            }

            if (write_buffer(STD_OUT_DECR, write_buf.get(), read_bytes, &status) == FAIL_CODE) {
                std::cerr << "Error: while try to write to stdout!" << std::endl;
                no_io_error = false;
                exit_code = EXIT_FAILURE;
            }
        } while (read_bytes == BUFFER_SIZE);
    }

    for (size_t f_index = 0; f_index < file_descriptors.size(); ++f_index) {
        if (close(file_descriptors[f_index]) == -1) {
            exit_code = EXIT_FAILURE;
            std::cerr << "Error: can not close file! " << file_list[f_index] << std::endl;
        }
    }
    return exit_code;
}

void print_help(const po::options_description &options_description) {
    std::cout << "Usage:\n  mycat [-h|--help] [-A] <file1> <file2> ... <fileN>\n" << options_description << std::endl;
}

int write_buffer(int fd, const char *buffer, ssize_t size, int *status) {
    ssize_t written_bytes = 0;
    while (written_bytes < size) {
        ssize_t written_now = write(fd, buffer + written_bytes, size - written_bytes);
        if (written_now == -1) {
            if (errno == EINTR)
                continue;
            else {
                *status = errno;
                return FAIL_CODE;
            }
        } else
            written_bytes += written_now;
    }
    return SUCCESS_CODE;
}

ssize_t read_buffer(int fd, char *buffer, ssize_t size, int *status) {
    ssize_t read_bytes = 0;
    while (read_bytes < size) {
        ssize_t read_now = read(fd, (void *) (buffer + read_bytes), size - read_bytes);
        if (read_now == FAIL_CODE) {
            if (errno == EINTR)
                continue;
            else {
                *status = errno;
                return FAIL_CODE;
            }
        } else if (read_now == 0) {
//            buffer[read_bytes] = 0; TODO: check if adding '\0' to the buffer is needed
            return read_bytes;
        } else
            read_bytes += read_now;
    }
//    buffer[read_bytes] = 0; TODO: check if adding '\0' to the buffer is needed
    return read_bytes;
}

unsigned char num_to_hex_char(const unsigned char n) {
    if (n < 10)
        return '0' + n;
    else if (n < 16)
        return 'A' + (n - 10);
    else
        return '?';
}

void write_as_hex(char *buf, unsigned char c) {
    buf[0] = '\\';
    buf[1] = 'x';
    buf[2] = static_cast<char>(num_to_hex_char(c >> 4u));
    buf[3] = static_cast<char>(num_to_hex_char(c & 0x0fu));
}
