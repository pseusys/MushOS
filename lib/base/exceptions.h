#ifndef MUSHLIB_EXCEPTIONS_H
#define MUSHLIB_EXCEPTIONS_H

#include "generic.h"


typedef struct {
    u_byte id;
    string type;
    string message;
    string file;
    u_dword line;
} exception;

typedef void (*exception_handler)(exception exc);

void throw_exception(exception exc);
void handle_exceptions(u_byte id, exception_handler);

void terminate(u_byte code);

#define throw_verbose(id, type, message) {\
    exception exc = {id, type, message, __FILE__, __LINE__};\
    throw_exception(exc);\
}

#define throw_typed(id, type) throw_verbose(id, type, nullptr)

#define throw(id) throw_typed(id, nullptr)

#endif // MUSHLIB_EXCEPTIONS_H
