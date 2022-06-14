#ifndef DECODE_H
#define DECODE_H

#include "common.h"
#include "types.h"
#include <stdlib.h>
#define DEFAULT_SECRET_FILE_NAME "output."
#define DEFAULT_SECRET_FILE_NAME_LENGTH 12

typedef struct _DecodeInfo
{
    /* Source image Info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint raw_image_size;
    uint pixel_array_begin;
    
    /*secret file Info */
    char *secret_fname;
    char extn_secret_file[FILE_EXTENTION_SIZE];
    FILE *fptr_secret_file;
    uint secret_file_size;
} DecodeInfo;

/* read and validate decoding arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
/* check magic string */

/*get secret file's size and extention*/
Status decode_secret_extn(DecodeInfo *decInfo);

/* do decoding */
Status do_decoding(DecodeInfo *decInfo);

#endif
