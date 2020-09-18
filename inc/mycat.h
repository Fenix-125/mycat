//
// Created by fenix on 14.09.20.
//

#ifndef MYCAT_MYCAT_H
#define MYCAT_MYCAT_H

#include <string>
#include <vector>
#include <mycat_opts.h>
#include <defines.h>


namespace ucu {
    class mycat {
    public:
        mycat() = delete;

        explicit mycat(const mycat_opts &opts);

        ~mycat();

        void run();

        static void flush_buffer(const char *start, ssize_t count, int f_desc = STD_OUT_DECR);

    protected:
        std::unique_ptr<char[]> read_buf;
        const std::vector<std::string> &file_list;

        std::vector<int> file_descriptors{};

        virtual void print_file(int f_desc, const std::string &f_name);

    private:
        const mycat_opts &opts;

        void open_files();

        void close_files();

    };

    class mycat_error : public std::exception {
    public:
        [[nodiscard]] const char *what() const noexcept override {
            return "Error: Error in mycat!";
        }
    };
}

#endif //MYCAT_MYCAT_H
