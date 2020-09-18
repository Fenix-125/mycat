//
// Created by fenix on 18.09.20.
//

#include <mycat_hex.h>
#include <iostream>
#include <syscall_io.h>
#include <char_to_hex.h>
#include <defines.h>

ucu::mycat_hex::mycat_hex(const ucu::mycat_opts &opts) : mycat(opts) {
    write_buf.reset(new char[BUFFER_SIZE + 1]);
    write_buf.get()[BUFFER_SIZE] = 0;
}

void ucu::mycat_hex::print_file(int f_desc, const std::string &f_name) {
    int status = 0;
    ssize_t read_bytes;

    do {
        read_bytes = ucu::read_buffer(f_desc, read_buf.get(), BUFFER_SIZE, &status);
        if (read_bytes == FAIL_CODE) {
            std::cerr << "Error: while read form file: " << f_name << std::endl;
            throw ucu::mycat_error{};
        }

        for (char *rd_start = read_buf.get(), *wr_index = write_buf.get(), *rd_end = read_buf.get();
             rd_end < read_buf.get() + read_bytes; ++rd_end) {
            const auto &c = *rd_end;
            if (wr_index + (rd_end - rd_start - 1) + HEX_STR_SIZE + 1 > write_buf.get() + BUFFER_SIZE) {
                // should not overlap as it is the first condition in the loop
                std::memmove(wr_index, rd_start, rd_end - rd_start - 1);
                wr_index += rd_end - rd_start - 1;
                rd_start = rd_end - 1;
                mycat::flush_buffer(write_buf.get(), wr_index - write_buf.get());
                wr_index = write_buf.get();
            }

            // Finalize
            if (rd_end + 1 == read_buf.get() + read_bytes) {
                std::memmove(wr_index, rd_start, rd_end - rd_start + 1);
                wr_index += rd_end - rd_start + 1;
                *wr_index = 0;
                mycat::flush_buffer(write_buf.get(), wr_index - write_buf.get());
            } else if (!isspace(static_cast<int>(c)) && !isprint(static_cast<int>(c))) {
                if (rd_end - rd_start > 0) {
                    std::memmove(wr_index, rd_start, rd_end - rd_start);
                    wr_index += rd_end - rd_start;
                }
                ucu::write_as_hex(wr_index, c);
                wr_index += HEX_STR_SIZE;
                rd_start = rd_end + 1;
            }
        }

    } while (read_bytes == BUFFER_SIZE);
}
