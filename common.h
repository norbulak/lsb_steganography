#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include "debug.h"
/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"
#define MAGIC_STRING_SIZE 2
#define PRINT(t, fmt, ...)\
    do {\
            if (t == DEBUG_PRINT) \
            {\
                if (DEBUG) fprintf(stderr, "%s:%d:%s(): ", fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__);kkk\
       } while (0)

#endif
