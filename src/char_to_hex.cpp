//
// Created by fenix on 18.09.20.
//

#include "char_to_hex.h"


unsigned char ucu::num_to_hex_char(const unsigned char n) {
    if (n < 10)
        return '0' + n;
    else if (n < 16)
        return 'A' + (n - 10);
    else
        return '?';
}

void ucu::write_as_hex(char *buf, unsigned char c) {
    buf[0] = '\\';
    buf[1] = 'x';
    buf[2] = static_cast<char>(num_to_hex_char(c >> 4u));
    buf[3] = static_cast<char>(num_to_hex_char(c & 0x0fu));
}
