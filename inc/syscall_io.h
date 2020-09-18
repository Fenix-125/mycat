//
// Created by fenix on 18.09.20.
//

#ifndef MYCAT_SYSCALL_IO_H
#define MYCAT_SYSCALL_IO_H

#include <cstdlib>
#include <defines.h>


namespace ucu {
    int write_buffer(int fd, const char *buffer, ssize_t size, int *status);

    ssize_t read_buffer(int fd, char *buffer, ssize_t size, int *status);
}

#endif //MYCAT_SYSCALL_IO_H
