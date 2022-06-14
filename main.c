#include <stdio.h>
#include "debug.h"
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"
int main(int argc, char *argv[])
{
    unsigned int img_size;

    // Fill with sample filenames
    switch (check_operation_type(argv)) 
    {
        case e_encode:
            EncodeInfo encInfo;
            if(e_failure == read_and_validate_encode_args(argv, &encInfo))
                ERROR("Invalid Arguments\n");
            if(e_failure == open_encoding_files(&encInfo))
            {
                ERROR("Opening Files Failed\n");
                return e_failure;
            }
            do_encoding(&encInfo);
            close_encoding_files(&encInfo);
            break;
        case e_decode:
            DecodeInfo decInfo;
            if(e_failure == read_and_validate_decode_args(argv, &decInfo))
                    ERROR("Invalid Arguments\n");
                    do_decoding(&decInfo);
            //              close_decoding_file(&decInfo)
            break;
        default:
            return -1;
    }            
    return e_success;
}
