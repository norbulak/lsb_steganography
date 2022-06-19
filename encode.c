#include "encode.h"
#include "common.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
Status close_encoding_files(EncodeInfo *encInfo)
{
    INFO("Closing files\n")
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
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
    INFO("Usage:\n"
           "Encoding: stego -e <.bmp file> <secret file> [output file]\n"
           "Decoding: stego -d <.bmp file> <secret file> [output file]\n");
    return e_unsupported;
}

void get_secret_extention(EncodeInfo *encInfo)
{
    DEBUG("secret_fname= %s\n", encInfo->secret_fname);
    strtok(encInfo->secret_fname, ".");
    char *token = strtok(NULL, ".");
    strcpy(encInfo->extn_secret_file, token);
    DEBUG("Token = %s\n", encInfo->extn_secret_file);
    return;
}

Status read_and_validate_encode_args(char **argv, EncodeInfo *encInfo)
{
    // check if options are provided properly
    if (NULL == argv[2] || NULL == argv[3]
    || strcmp(".bmp", (argv[2] + strlen(argv[2]) - 4)) != 0)
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

        encInfo->stego_image_fname = (char *)DEFAULT_STEGO_FILE_NAME;
    }

    return e_success;
usage:
    ERROR("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n"
            , argv[0], argv[0]);
    return e_failure;
}

uint get_file_size(FILE *fptr)
{
    uint size = 0;
    fseek(fptr, 0L, SEEK_END);
    size = ftell(fptr);
    rewind(fptr);
    return (size);
}
Status check_capacity(EncodeInfo *encInfo)
{
    // getting number of pixels
    fseek(encInfo->fptr_src_image, BMP_RAW_IMG_SIZE_OFFSET, SEEK_SET);
    fread(&encInfo->raw_image_size, 4, 1, encInfo->fptr_src_image);
    DEBUG("Raw image size = %d\n", encInfo->raw_image_size);
    encInfo->image_capacity = encInfo->raw_image_size/8;
    DEBUG("image capacity = %d bytes\n", encInfo->image_capacity);

    // Comparing the secret message's size with the capacity of the BMP
    // capacity is equal to one byte per pixel for 24bbp BMPs
    if (FILE_EXTENTION_SIZE + BYTES_OF_FILE_SIZE +
    MAGIC_STRING_SIZE + encInfo->size_secret_file > encInfo->image_capacity)
    {
        ERROR("%s is too small to contain %s\n", 
                encInfo->src_image_fname, encInfo->secret_fname);
        return e_failure;
    }
    return e_success;
}

Status check_bitmap_format(EncodeInfo *encInfo)
{
    INFO("Checking that the source bitmap is 24bbp\n");
    fseek(encInfo->fptr_src_image, BMP_BITS_PER_PIXELS_OFFSET, SEEK_SET);
    fread(&encInfo->bits_per_pixel, 2, 1, encInfo->fptr_src_image);
    if (24 != encInfo->bits_per_pixel)
    {
        ERROR("%s is not a 24bbp bitmap\n", encInfo->src_image_fname);
        return e_failure;
    }
    INFO("BMP file is 24bbp\n");
    return e_success;
}

Status copy_bmp_header(EncodeInfo *encInfo)
{
    //Copy source file into new file until beginning of pixel ARRAY
    INFO("Copying the file until 0x%x\n", encInfo->pixel_array_begin);
    rewind(encInfo->fptr_src_image);
    char tmp_data[encInfo->pixel_array_begin];
    fread(tmp_data, encInfo->pixel_array_begin, 1, encInfo->fptr_src_image);
    fwrite(&tmp_data, encInfo->pixel_array_begin, 1, encInfo->fptr_stego_image);
    DEBUG("Created the buffer = %x\n", encInfo->pixel_array_begin);
    INFO("Done.\n")
    return e_success;
}

Status lsb_encode(char *buffer, char byte)
{
    unsigned char mask = 0x80;
    bool lsb = 0;
    for (int i = 0; i < 8; ++i)
    {
        lsb = byte & mask;
        if (lsb)
            buffer[i] |= (unsigned char)1;
        else
            buffer[i] &= (unsigned char)~1;
        mask >>=1;
    }

    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    INFO("## Encoding started\n")
    // check if 24bbp
    if (e_failure == check_bitmap_format(encInfo))
        return e_failure;

    // Checking secret if file is empty
    INFO("Checking %s size\n", encInfo->secret_fname);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    DEBUG("Secret file size = %ld Bytes\n", encInfo->size_secret_file);
    if (0 == encInfo->size_secret_file)
    {
        ERROR("No data in secret file\n");
        return e_failure;
    }
    INFO("Done. Not empty\n")

    //checking image capacity 
    INFO("Checking image capacity...\n");
    if (e_failure == check_capacity(encInfo))
        return e_failure;
    INFO("Done. Sufficient !\n");

    //Getting secret file extention
    get_secret_extention(encInfo);
    //Get secret message to encode
    INFO("Getting secret message\n");
    encInfo->secret_data = malloc(sizeof(char) * encInfo->size_secret_file);
    fread(encInfo->secret_data, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    INFO("Done.\n");

    // getting starting offset of the bmp's pixel array
    fseek(encInfo->fptr_src_image, BMP_PIXEL_ARRAY_START_OFFSET, SEEK_SET);
    fread(&encInfo->pixel_array_begin, 4, 1, encInfo->fptr_src_image);

    // Copy bmp header
    if (e_failure == copy_bmp_header(encInfo))
        return e_failure;

    // Load image to memory
    fseek(encInfo->fptr_src_image, encInfo->pixel_array_begin, SEEK_SET); 
    char *tmp_buffer = malloc(encInfo->raw_image_size);
    fread(tmp_buffer, encInfo->raw_image_size , 1, encInfo->fptr_src_image);

    // encode MAGIC_STRING
    int j = 0;
    INFO("Encoding Magic String...\n");

    for(int i = 0; i < MAGIC_STRING_SIZE; ++i)
    {
        lsb_encode(&tmp_buffer[j], MAGIC_STRING[i]);
        j+=8;
    }

    INFO("Done\n");
    // Encoding secret file size on 4 bytes
    for(int i = BYTES_OF_FILE_SIZE; i > 0;)
    {
        lsb_encode(&tmp_buffer[j], (encInfo->size_secret_file >> (--i)*8) & 0xff);
        j+=8;
    }
    // Encoding secret file extension(to know type when decoding)
    for(int i = 0; i < FILE_EXTENTION_SIZE;++i)
    {
        lsb_encode(&tmp_buffer[j], encInfo->extn_secret_file[i]);
        j+=8;
    }
    INFO("Encoding Secret Data...\n");
    for(int i = 0; i < encInfo->size_secret_file  ; ++i)
    {
        lsb_encode(&tmp_buffer[j], encInfo->secret_data[i]);
        j+=8;
    }
    INFO("Data Encoded\n")
    INFO("Writing encoded DATA to file...\n");
    fwrite(tmp_buffer, encInfo->raw_image_size , 1, encInfo->fptr_stego_image);
    INFO("Done\n")
    free(tmp_buffer);
    uint img_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);

    return e_failure;
}
