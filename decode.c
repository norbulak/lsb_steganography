#include "decode.h"
#include <string.h>

Status read_and_validate_decode_args(char **argv, DecodeInfo *decInfo)
{
    // check if options are provided properly
    if (NULL == argv[2]
    || strcmp(".bmp", (argv[2] + strlen(argv[2]) - 4)) != 0)
        goto usage;

    // save bitmap & secret_fname file name to encodeInfo 
    decInfo->src_image_fname = argv[2];

    if (NULL != argv[3])
        decInfo->secret_fname= argv[3];
    else
    {
        INFO("Output File not mentioned. Creating %s as default\n",
                DEFAULT_SECRET_FILE_NAME);

        decInfo->secret_fname = (char *)DEFAULT_SECRET_FILE_NAME;
        strcat(decInfo->secret_fname, decInfo->extn_secret_file);
    }

    return e_success;
usage:
    ERROR("%s: Decoding: %s -d <.bmp file> [output file name]\n"
            , argv[0], argv[0]);
    return e_failure;
}

Status do_decoding(DecodeInfo *decInfo)
{
    // get pixel array start
    INFO("## Decoding Started...\n");
    //open bitmap 
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    INFO("Opened %s\n", decInfo->src_image_fname);
    //Do Error handling
    if (NULL == decInfo->fptr_src_image)
    {
        perror("fopen");
        ERROR("ERROR: Unable to open file %s\n", decInfo->src_image_fname);
        return e_failure;
    }
    
    // getting raw image size
    fseek(decInfo->fptr_src_image, BMP_RAW_IMG_SIZE_OFFSET, SEEK_SET);
    fread(&decInfo->raw_image_size, 4, 1, decInfo->fptr_src_image);
    // getting starting offset of the bmp's pixel array
    fseek(decInfo->fptr_src_image, BMP_PIXEL_ARRAY_START_OFFSET, SEEK_SET);
    fread(&decInfo->pixel_array_begin, 4, 1, decInfo->fptr_src_image);
    
    //read pixel array 

    //check if magic string is present
     

}
