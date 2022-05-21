#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    DEBUG("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    DEBUG("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_encoding_files(EncodeInfo *encInfo)
{
    INFO("Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    INFO("Opened %s\n", encInfo->src_image_fname);
    //Do Error handling
    if (NULL == encInfo->fptr_src_image)
    {
        perror("fopen");
        ERROR("ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    INFO("Opened %s\n", encInfo->secret_fname);
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        ERROR("Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    INFO("Opened %s\n", encInfo->stego_image_fname);
    //Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        ERROR("Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    // No failure return e_success
    INFO("Done\n")
    return e_success;
}

OperationType check_operation_type(char **argv)
{
    if (NULL != argv[1])
    {
        if (strcmp(argv[1], "-e") == 0)
            return e_encode;
        else if (strcmp(argv[1], "-d") == 0)
            return e_decode;
    }
    printf("Usage:\n"
           "stego: Encoding: stego -e <.bmp file> <.txt file> [output file]\n"
           "stego: Decoding: stego -d <.bmp file> <.txt file> [output file]\n");
    return e_unsupported;
}

Status read_and_validate_encode_args(char **argv, EncodeInfo *encInfo)
{
    // check if options are provided properly
    if (NULL == argv[2] || NULL == argv[3]
    || strcmp(".bmp", (argv[2] + strlen(argv[2]) - 4)) != 0
    || strcmp(".txt", (argv[3] + strlen(argv[3]) - 4)) != 0)
        goto usage;

    // save bitmap & secret_fname file name to encodeInfo 
    encInfo->src_image_fname = argv[2];
    encInfo->secret_fname= argv[3];

    if (NULL != argv[4])
        encInfo->stego_image_fname = argv[4];
    else
    {
        INFO("Output File not mentioned. Creating %s as default\n",
                DEFAULT_STEGO_FILE_NAME);

        encInfo->stego_image_fname = malloc(DEFAULT_STEGO_FILE_NAME_SIZE);
        strcpy(encInfo->stego_image_fname, DEFAULT_STEGO_FILE_NAME);
    }

    return e_success;
usage:
    ERROR("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n"
            , argv[0], argv[0]);
    return e_failure;
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("##Decoding Procedure Started##\n");
    // Checking secret file size (Check if empty)
    INFO("Checking %s size\n", encInfo->secret_fname);
    fseek(encInfo->fptr_secret, 0L, SEEK_END);
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);
    if (0 == encInfo->size_secret_file)
    {
        ERROR("No data in secret file\n");
        return e_failure;
    }
    DEBUG("Size of secret file is %ld\n", encInfo->size_secret_file);
    INFO("Done. Not empty\n")

    // checking if enough space to write magic string+secret message in bmp
    INFO("Checking for %s capacity to handle %s\n",
            encInfo->src_image_fname, encInfo->secret_fname);
    if (MAGIC_STRING_SIZE+encInfo->size_secret_file > encInfo->image_capacity)
    {
        ERROR("%s is too small to contain %s\n", 
                encInfo->src_image_fname, encInfo->secret_fname);
        return e_failure;
    }
    INFO("Done. Found OK\n");
    fseek(encInfo->fptr_src_image, BMP_BITS_PER_PIXELS_OFFSET, SEEK_SET);
    fread(&encInfo->bits_per_pixel, 2, 1, encInfo->fptr_src_image);

    // seeking from current to optimize
    fseek(encInfo->fptr_src_image, BMP_RAW_IMG_SIZE_OFFSET, SEEK_SET);

    fread(&encInfo->image_capacity, 4, 1, encInfo->fptr_src_image);

    DEBUG("image capacity = %d bytes\n", encInfo->image_capacity);
    DEBUG("%d bbp\n", encInfo->bits_per_pixel);

    // checking for bmp capacity to encode message 
    // Copy image header (overhead before that doesn't change)
         
    // Encoding MAGIC_STRING signature
    // Encoding Secret file extension
    // Encoding Secret file Size
    // Encoding Secret data
    // Copying the data that's left on the original bmp (that doesn't change)
    // Done
    // get_image_size_for_bmp
    uint img_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);

    return e_failure;
}
