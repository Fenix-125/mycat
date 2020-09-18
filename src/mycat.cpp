//
// Created by fenix on 14.09.20.
//

#include <mycat.h>
#include <iostream>
#include <syscall_io.h>
#include <defines.h>

#include <fcntl.h> // types and constants for fcntl() and open()
#include <unistd.h> // POSIX header


ucu::mycat::mycat(const mycat_opts &opts) : file_list(opts.get_file_list()), opts(opts) {
    open_files();
    read_buf.reset(new char[BUFFER_SIZE + 1]);
    read_buf.get()[BUFFER_SIZE] = 0;
}

ucu::mycat::~mycat() {
    close_files();
}

void ucu::mycat::open_files() {
    for (size_t f_index = 0; f_index < file_list.size(); ++f_index) {
        file_descriptors.emplace_back(open(file_list[f_index].c_str(), O_RDONLY));
        if (file_descriptors[f_index] == FAIL_CODE) {
            close_files();
        }
    }
}

void ucu::mycat::close_files() {
    bool err_showed = false;
    for (size_t f_index = 0; f_index < file_descriptors.size(); ++f_index) {
        if (close(file_descriptors[f_index]) == FAIL_CODE) {
            std::cerr << "Error: can not close file! " << file_list[f_index] << std::endl;
            err_showed = true;
        }
    }
//    file_descriptors.clear();
    if (err_showed)
        throw mycat_error{};
}

void ucu::mycat::run() {
    for (size_t f_index = 0; f_index < file_descriptors.size(); ++f_index) {
        print_file(file_descriptors[f_index], file_list[f_index]);
    }
}

void ucu::mycat::print_file(int f_desc, const std::string &f_name) {
    int status = 0;
    ssize_t read_bytes;

    do {
        read_bytes = ucu::read_buffer(f_desc, read_buf.get(), BUFFER_SIZE, &status);
        if (read_bytes == FAIL_CODE) {
            std::cerr << "Error: while read form file: " << f_name << std::endl;
            throw ucu::mycat_error{};
        }
        flush_buffer(read_buf.get(), read_bytes);
    } while (read_bytes == BUFFER_SIZE);
}

void ucu::mycat::flush_buffer(const char *start, ssize_t count, int f_desc) {
    int status;
    if (ucu::write_buffer(f_desc, start, count, &status) == FAIL_CODE) {
        std::cerr << "Error: while try to write to stdout!" << std::endl;
        throw ucu::mycat_error{};
    }
}
