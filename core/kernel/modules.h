#ifndef MUSHOS_MODULES_H
#define MUSHOS_MODULES_H

#include "../../lib/base/generic.h"


void init_module_loading_driver();

void* load_module(u_word offset, u_word length);

#endif //MUSHOS_MODULES_H
