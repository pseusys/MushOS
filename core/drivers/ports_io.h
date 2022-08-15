#ifndef MUSHOS_PORTS_IO_H
#define MUSHOS_PORTS_IO_H

#include <mushlib.h>

// Corresponding assembly functions defined in ports_io.asm

extern byte port_byte_in (u_word port);
extern void port_byte_out (u_word port, byte data);
extern u_word port_word_in (u_word port);
extern void port_word_out (u_word port, u_word data);

#endif //MUSHOS_PORTS_IO_H
