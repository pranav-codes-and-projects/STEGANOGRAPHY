//Name : Atluri Pranav Karthik
//Project : STEGANOGRAPHY
//Date of completion : 01-09-2026
//Date of submission : 07-09-2026
#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"


// Read and validate decoding arguments ./a.out -d stego.bmp
 
Status read_and_validate_decode_args(char *argv[],
                                     DecodeInfo *decInfo)
{
    // Step 1: Check argv[2] has .bmp

    if (argv[2] == NULL)
    {
        return e_failure;
    }

    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    

    return e_success;
}


//Open stego image

Status open_decode_file(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image =
        fopen(decInfo->stego_image_fname, "r");

    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");

        printf("ERROR: Unable to open %s\n",
               decInfo->stego_image_fname);

        return e_failure;
    }

    return e_success;
}


//Decode one byte from 8 image bytes
Status decode_byte_from_lsb(char *data,char *image_buffer)
{
    int i;

    *data = 0;

    for (i = 0; i < 8; i++)
    {
        /*
         * Make space for next bit
         */
        *data = *data << 1;

        /*
         * Take LSB from image byte
         * and put into data
         */
        *data = *data | (image_buffer[i] & 1);
    }

    return e_success;
}


// Decode 32-bit integer  from 32 image bytes
Status decode_size_from_lsb(int *size,
                            char *image_buffer)
{
    int i;

    *size = 0;

    for (i = 0; i < 32; i++)
    {
        // Shift existing bits
        *size = *size << 1;

        // Take LSB of image byte
        *size = *size | (image_buffer[i] & 1);
    }

    return e_success;
}


//Decode magic string

Status decode_magic_string(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;
    char decoded_magic[20];

    int i;

    // MAGIC_STRING = "#*".Therefore decode strlen(MAGIC_STRING) characters.


    for (i = 0; i < strlen(MAGIC_STRING); i++)
    {
        //Read 8 image bytes
        
        if (fread(buffer, 8, 1,decInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }

        //Decode one character

        decode_byte_from_lsb(&ch, buffer);

        decoded_magic[i] = ch;
    }

    decoded_magic[i] = '\0';


    //Compare decoded magic string with original MAGIC_STRING

    if (strcmp(decoded_magic, MAGIC_STRING) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


//Decode secret file extension size

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];

    //Read 32 image bytes

    if (fread(buffer, 32, 1,decInfo->fptr_stego_image) != 1)
    {
        return e_failure;
    }

    //Decode extension size

    decode_size_from_lsb(&decInfo->extn_size,buffer);

    return e_success;
}


//Decode secret file extension

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    int i;

    //Decode extn_size characters
    for (i = 0; i < decInfo->extn_size; i++)
    {
        //Read 8 bytes

        if (fread(buffer, 8, 1,decInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }

        //Decode one character

        decode_byte_from_lsb(&ch, buffer);

        //Store decoded character

        decInfo->extn_secret_file[i] = ch;
    }

    //Add null character

    decInfo->extn_secret_file[i] = '\0';

    return e_success;
}




Status create_output_file(char *argv[], DecodeInfo *decInfo)
{
    if (argv[3] != NULL)
    {
        strcpy(decInfo->output_fname, argv[3]);
    }
    else
    {
        strcpy(decInfo->output_fname, "decoded");
    }

    // Add the extension obtained during decoding 
    strcat(decInfo->output_fname, decInfo->extn_secret_file);

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");

    if (decInfo->fptr_output == NULL)
    {
        printf("ERROR: Unable to create output file\n");
        return e_failure;
    }

    return e_success;
}


// Decode secret file size

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    int size;

    // Read 32 image bytes

    if (fread(buffer, 32, 1,decInfo->fptr_stego_image) != 1)
    {
        return e_failure;
    }

    //Decode size

    decode_size_from_lsb(&size,buffer);

    decInfo->size_secret_file = size;

    return e_success;
}


// Decode secret file data

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    long i;

    //Repeat for size_secret_file bytes

    for (i = 0;i < decInfo->size_secret_file;i++)
    {
        //Step 1: Read 8 image bytes
    
        if (fread(buffer, 8, 1,decInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }


        //Step 2: Decode one byte

        decode_byte_from_lsb(&ch,buffer);


        //Step 3: Write decoded byte into output file

        if (fwrite(&ch, 1, 1,decInfo->fptr_output) != 1)
        {
            return e_failure;
        }
    }

    return e_success;
}


// Complete decoding process
Status do_decoding(char *argv[],DecodeInfo *decInfo)
{
    // Step 1: Open stego image

    if (open_decode_file(decInfo) == e_failure)
    {
        printf("ERROR: Unable to open stego image\n");

        return e_failure;
    }
    else
    {
        printf("Stego image opened successfully\n");
    }


    /*
     * IMPORTANT
     *
     * First 54 bytes are BMP header.
     * Encoding started after 54 bytes.
     *
     * So decoder must also move
     * to byte 54.
     */

    fseek(decInfo->fptr_stego_image,54,SEEK_SET);


    //Step 2:Decode magic string

    if (decode_magic_string(decInfo) == e_failure)
    {
        printf("ERROR: Magic string not found\n");

        return e_failure;
    }
    else
    {
        printf("Magic string decoded successfully\n");
    }


    //Step 3:Decode extension size

    if (decode_secret_file_extn_size(decInfo)
        == e_failure)
    {
        printf("ERROR: Unable to decode extension size\n");

        return e_failure;
    }
    else
    {
        printf("Secret file extension size decoded\n");
    }


    //Step 4: Decode extension

    if (decode_secret_file_extn(decInfo)
        == e_failure)
    {
        printf("ERROR: Unable to decode extension\n");

        return e_failure;
    }
    else
    {
        printf("Secret file extension decoded: %s\n",decInfo->extn_secret_file);
    }


    //Step 5: Create output file

    if (create_output_file(argv,decInfo) == e_failure)
    {
        printf("ERROR: Output file creation failed\n");

        return e_failure;
    }
    else
    {
        printf("Output file created: %s\n", decInfo->output_fname);
    }


    // Step 6:  Decode secret file size

    if (decode_secret_file_size(decInfo) == e_failure)
    {
        printf("ERROR: Unable to decode secret file size\n");

        return e_failure;
    }
    else
    {
        printf("Secret file size decoded: %ld bytes\n",decInfo->size_secret_file);
    }


    //Step 7:Decode secret file data
    

    if (decode_secret_file_data(decInfo) == e_failure)
    {
        printf("ERROR: Unable to decode secret file data\n");

        return e_failure;
    }
    else
    {
        printf("Secret file data decoded successfully\n");
    }


    //Close files
    

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);


    printf("Decoding completed successfully\n");

    return e_success;
}