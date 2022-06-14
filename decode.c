#include "decode.h"
#include "debug.h"
#include <string.h>

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // check if options are provided properly
    INFO("checking decoding arguments\n")
    if (NULL == argv[2]
    || strcmp(".bmp", (argv[2] + strlen(argv[2]) - 4)) != 0)
        goto usage;

    // save bitmap & secret_fname file name to encodeInfo 
    decInfo->src_image_fname = argv[2];

    if (NULL != argv[3])
        decInfo->secret_fname= argv[3];
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
    DEBUG("strlen%s\n", decInfo->extn_secret_file);
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
    
    // Load image to memory
    fseek(decInfo->fptr_src_image, decInfo->pixel_array_begin, SEEK_SET); 
    char *tmp_buffer = malloc(decInfo->raw_image_size);
    fread(tmp_buffer, decInfo->raw_image_size , 1, decInfo->fptr_src_image);

    //check if magic string is present
    int j = 7;
    for(int i = 0; i < MAGIC_STRING_SIZE; ++i)
    {
        if (tmp_buffer[j] != MAGIC_STRING[i]
          ||tmp_buffer[j+1] != MAGIC_STRING[++i]) 
        {
            ERROR("Magic string not present\n");
            return e_failure;
        }
        j+=8;
    }
    INFO("Magic string present\n");

    //get secret file size
    decInfo->secret_file_size = 0;
    for (int i = 0; i < BYTES_OF_FILE_SIZE; ++i)
    {
        decInfo->secret_file_size <<= 8;
        decInfo->secret_file_size |= tmp_buffer[j] & 0xff;
        decInfo->secret_file_size <<= 8;
        decInfo->secret_file_size |= tmp_buffer[j+1] & 0xff;
        ++i;
        j+=8;
    }

    // get secret file extention
    for(int i = 0; i < FILE_EXTENTION_SIZE;++i)
    {
        decInfo->extn_secret_file[i]  = tmp_buffer[j];
        DEBUG("File extention byte = %c\n", tmp_buffer[j]);
        decInfo->extn_secret_file[++i]  = tmp_buffer[j+1];
        DEBUG("File extention byte = %c\n", tmp_buffer[j+1]);
        j+=8;
    }
    DEBUG("decInfo->extn_secret_file = %s\n", decInfo->extn_secret_file);

    // create and open output file.
    DEBUG("secret_fname = %s\n",decInfo->secret_fname);
    if (NULL == decInfo->secret_fname)
    {
        decInfo->secret_fname = malloc(DEFAULT_SECRET_FILE_NAME_LENGTH * sizeof(char));
        strcpy(decInfo->secret_fname, DEFAULT_SECRET_FILE_NAME);
        strcat(decInfo->secret_fname, decInfo->extn_secret_file);
        DEBUG("Default secret file name %s\n", decInfo->secret_fname);
    }
    //Open output file
    INFO("Opening output file @ : %s\n", decInfo->secret_fname);
    decInfo->fptr_secret_file = fopen(decInfo->secret_fname, "w");
    //Do Error handling
    if (decInfo->fptr_secret_file == NULL)
    {
        perror("fopen");
        ERROR("Unable to open file %s\n", decInfo->secret_fname);
        return e_failure;
    }

    // save secret data
    char secret_data[decInfo->secret_file_size];
    for (int i = 0; i < decInfo->secret_file_size; ++i)
    {
        secret_data[i] = tmp_buffer[j];
        secret_data[++i] = tmp_buffer[j+1];
        j+=8;
    }
    DEBUG("Secret data = %s\n", secret_data);

    // write secret data to file
    fwrite(secret_data, decInfo->secret_file_size, 1, decInfo->fptr_secret_file);

    // close files
    fclose(decInfo->fptr_src_image);
    fclose(decInfo->fptr_secret_file);
    return e_success;
}
