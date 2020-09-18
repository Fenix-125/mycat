//
// Created by fenix on 18.09.20.
//

#ifndef MYCAT_MYCAT_HEX_H
#define MYCAT_MYCAT_HEX_H

#include <mycat.h>

namespace ucu {
    class mycat_hex : public ucu::mycat {
    public:
        mycat_hex() = delete;

        explicit mycat_hex(const mycat_opts &opts);

    private:
        std::unique_ptr<char[]> write_buf;

        void print_file(int f_desc, const std::string &f_name) override;
    };
}


#endif //MYCAT_MYCAT_HEX_H
