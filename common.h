#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include "debug.h"
#include <string.h>
#include "debug.h"
#include "types.h"
// Secret file suffix max size
#define DEFAULT_EXT     "txt"
// Magic string to identify whether stegged or not
#define MAGIC_STRING "#*"
#define MAGIC_STRING_SIZE 2
# define PRINT(t, fmt, ...)\
    do {\
            if (t == DEBUG_PRINT) \
            {\
                if (DEBUG) fprintf(stderr, "%s:%d:%s(): ", fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__);\
       } while (0)

# endif
#define BYTES_OF_FILE_SIZE              4
#define FILE_EXTENTION_SIZE             4  

// BMP offsets
#define BMP_BITS_PER_PIXELS_OFFSET      0x1C    // 4 bytes
#define BMP_WIDTH_IN_PIXELS_OFFSET      0x12    // 4 bytes
#define BMP_HEIGHT_IN_PIXELS_OFFSET     0x14    // 2 bytes
#define BMP_RAW_IMG_SIZE_OFFSET         0x22    // 4 bytes
#define BMP_PIXEL_ARRAY_START_OFFSET    0x0A    // 4 bytes

/* Get file size */
uint get_file_size(FILE *fptr);
