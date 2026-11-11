//Name : Atluri Pranav Karthik
//Project : STEGANOGRAPHY
//Date of completion : 01-09-2026
//Date of submission : 07-09-2026

#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[]) // .aout -e <source_file> <secret_data_file> <optional>
{

    if(check_operation_type(argv[1]) == e_encode)
    {
        EncodeInfo encinfo;
        if(read_and_validate_encode_args(argv,&encinfo) == e_failure)
        {
             printf("ERROR: Invalid arguments\n");
            return 1;
        }

        if (do_encoding(&encinfo) == e_failure)
        {
            printf("ERROR: Encoding failed\n");
            return 1;
        }
         printf("Encoding successful\n");
    }
    else if (check_operation_type(argv[1]) == e_decode)
    {
        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argv,&decInfo) == e_failure)
        {
            printf("ERROR: Invalid decoding arguments\n");

            return 1;
        }
        else
        {
            if (do_decoding(argv,&decInfo) == e_failure)
            {
                printf("Decoding failed\n");

                return 1;
            }
        }
    }

    else
    {
        printf("ERROR: Unsupported operation\n");
        return 1;
    }

        return 0;
    }
    //step 1 -> check check_operation_type(argv[1]) is returning e_encode or not
        // yes -> EncodeInfo encInfo;
            // check read_and_validate_encode_args(argv, &encInfo) is returning e_success or e_failure
                // failure -> print error msg and stop
                // success -> 

OperationType check_operation_type(char *symbol)
{
    if(strcmp(symbol, "-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(symbol, "-d")==0)
    {
        return e_decode;
    }
    else
    {
       return e_unsupported;
    }
    //step 1 -> check symbol is -e or not
        // yes -> return e_encode
    
    // step 2 -> check symbol is -d or not
        // yes -> return e_decode

    // return e_unsupported
}
