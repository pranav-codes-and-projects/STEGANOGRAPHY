//Name : Atluri Pranav Karthik
//Project : STEGANOGRAPHY
//Date of completion : 01-09-2026
//Date of submission : 07-09-2026
#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    //Stego image 
    char *stego_image_fname;
    FILE *fptr_stego_image;

    // Output file 
    char output_fname[100];
    FILE *fptr_output;

    // Decoded extension 
    char extn_secret_file[20];

    // Size of extension 
    int extn_size;

    // Size of secret file
    long size_secret_file;

} DecodeInfo;
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);
Status open_decode_file(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status create_output_file(char *argv[],DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status decode_byte_from_lsb(char *data,char *image_buffer);
Status decode_size_from_lsb(int *size,char *image_buffer);
Status do_decoding(char *argv[],DecodeInfo *decInfo);
#endif