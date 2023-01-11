#include "exceptions.h"

#include "stdio.h"


exception_handler handlers[256] = { [0 ... 255] = nullptr };


void throw_exception(exception exc) {
    if (handlers[exc.id] == nullptr) {
        bad("Exception #%d!", exc.id)
        if (exc.type != nullptr) bad(" Type: %s.\n", exc.type)
        if (exc.message != nullptr) bad("Message: %s\n", exc.message)
        bad("Code location: %s:%d\n", exc.file, exc.line)
        terminate(exc.id);
    } else handlers[exc.id](exc);
}

void handle_exceptions(u_byte id, exception_handler handler) {
    handlers[id] = handler;
}

void terminate(u_byte code) {
    // TODO: exit current app or PANIC if in kernel.
}
