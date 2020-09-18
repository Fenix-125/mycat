//
// Created by fenix on 18.09.20.
//

#include <syscall_io.h>
#include <unistd.h> // POSIX header
#include <cerrno>


int ucu::write_buffer(int fd, const char *buffer, ssize_t size, int *status) {
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

ssize_t ucu::read_buffer(int fd, char *buffer, ssize_t size, int *status) {
    ssize_t read_bytes = 0;
    while (read_bytes < size) {
        ssize_t read_now = read(fd, static_cast<void *>(buffer + read_bytes), size - read_bytes);
        if (read_now == FAIL_CODE) {
            if (errno == EINTR)
                continue;
            else {
                *status = errno;
                return FAIL_CODE;
            }
        } else if (read_now == 0) {
            return read_bytes;
        } else
            read_bytes += read_now;
    }
    return read_bytes;
}
