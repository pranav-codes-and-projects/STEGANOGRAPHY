//Name : Atluri Pranav Karthik
//Project : STEGANOGRAPHY
//Date of completion : 01-09-2026
//Date of submission : 07-09-2026
#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "common.h"
#include "types.h"
 
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    long current_position;
    long size;
     current_position = ftell(fptr);
     fseek(fptr,0,SEEK_END);
     size = ftell(fptr);
     fseek(fptr, current_position, SEEK_SET);
     return (uint)size;
    // Find the size of secret file data
    // return secret file size
}


//Get File pointers for i/p and o/p files.. Inputs: Src Image file, Secret file and Stego Image file
//Output: FILE pointer for above files
//Return Value: e_success or e_failure, on file errors
 

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2],".bmp")!=NULL)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("ERROR: Source image should be a .bmp file\n");
        return e_failure;
    }

    if(strchr(argv[3],'.')!=NULL)
    {
        encInfo->secret_fname=argv[3];
    }
    else
    {
         printf("ERROR: Secret file should have an extension\n");
        return e_failure;
    }
    if(argv[4]!=NULL)
    {
        if(strstr(argv[4],".bmp")!=NULL)
        {
          encInfo->stego_image_fname = argv[4];
        }
        else
        {
            printf("ERROR: Stego image should be a .bmp file\n");
          return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    // step 1 -> check argv[2] is having .bmp or not
        //yes -> store the file name into encInfo -> src_image_fname = argv[2]
        //no - > return e_failure

    // step 2 -> check argv[3] is having extn there or not
        // yes - > store the file name into encInfo -> secret_fname = argv[3]
        // no -> return e_failure
    
    // step 3 -> check argv[4] is NULL or not
        //yes -> check argv[4] is having .bmp or not
                //yes -> store the file name into encInfo -> stego_image_fname = argv[4]
                //no - > return e_failure
        
        //no -> store default name the encInfo -> stego_image_fname = "stego.bmp"


    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{ 
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > (16 + 32 + (strlen(encInfo->extn_secret_file)*8) + 32 + (encInfo -> size_secret_file * 8)))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
    //step 1 -> encInfo -> image_capacity = call get_image_size_for_bmp(encoInfo -> fptr_src_image)

    //step 2 -> encInfo -> size_secret_file = call get_file_size(encInfo -> fptr_secret)

    //step 3 -> check encInfo -> image_capacity > 16 + 32 + (strlen(encInfo -> extn_secret_file) * 8) + 32 + (encInfo -> size_secret_file * 8)
                //t -> return e_success;
                //f -> return e_failure
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{

    char buffer[54];

    rewind(fptr_src_image);

    if(fread(buffer,54,1,fptr_src_image)!=1)
    {
        return e_failure;
    }

    if (fwrite(buffer,54,1,fptr_dest_image)!=1)
    {
        return e_failure;
    }
    
    return e_success;

}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
      
    char buffer[8];
    int i;

    // Repeat for every character 
    for (i = 0; i < strlen(magic_string); i++)
    {
        // Step 1 -> read 8 bytes from source 
        if (fread(buffer, 8, 1,
                  encInfo->fptr_src_image) != 1)
        {
            return e_failure;
        }


        // Step 2 -> encode byte 
        if (encode_byte_to_lsb(magic_string[i],
                               buffer) == e_failure)
        {
            return e_failure;
        }


        // Step 3 -> write buffer 
        if (fwrite(buffer, 8, 1,
                   encInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }
    }
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{

    char buffer[32];

    // Repeat for every character 
        // Step 1 -> read 8 bytes from source 
        if (fread(buffer, 32, 1,encInfo->fptr_src_image) != 1)
        {
            return e_failure;
        }

        // Step 2 -> encode byte 
        if (encode_size_to_lsb(size,buffer) == e_failure)
        {
            return e_failure;
        }
        // Step 3 -> write buffer 
        if (fwrite(buffer, 32, 1,encInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{

    char buffer[8];
    int i;
    for (i = 0; i < strlen(file_extn); i++)
    {
        // Step 1 -> read 8 bytes from source 
        if (fread(buffer, 8, 1,encInfo->fptr_src_image) != 1)
        {
            return e_failure;
        }

        // Step 2 -> encode byte 
        if (encode_byte_to_lsb(file_extn[i],buffer) == e_failure)
        {
            return e_failure;
        }
        // Step 3 -> write buffer 
        if (fwrite(buffer, 8, 1,encInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    
    char buffer[32];

    // Repeat for every character 
        // Step 1 -> read 8 bytes from source 
        if (fread(buffer, 32, 1,encInfo->fptr_src_image) != 1)
        {
            return e_failure;
        }

        // Step 2 -> encode byte 
        if (encode_size_to_lsb(file_size,buffer) == e_failure)
        {
            return e_failure;
        }
        // Step 3 -> write buffer 
        if (fwrite(buffer, 32, 1,encInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }
    return e_success;

}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[8];
    char ch;
    // Repeat for every character 
    while(fread(&ch,1,1,encInfo->fptr_secret))
    {
        // Step 1 -> read 8 bytes from source 
        if (fread(buffer, 8, 1,encInfo->fptr_src_image) != 1)
        {
            return e_failure;
        }

        // Step 2 -> encode byte 
        if (encode_byte_to_lsb(ch,buffer) == e_failure)
        {
            return e_failure;
        }
        // Step 3 -> write buffer 
        if (fwrite(buffer, 8, 1,encInfo->fptr_stego_image) != 1)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[1024];
    int bytes;

    // Read remaining data 
    while ((bytes = fread(buffer, 1,sizeof(buffer),fptr_src)) > 0)
    {
        // Write remaining data 
        if (fwrite(buffer, 1, bytes,fptr_dest) != bytes)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i;
    int bit;
    for(i = 0;i<8;i++)
    {
        //Get one one bit from data
        bit = ((data >> (7-i))&1);
        
        //Clear one one bit from data
        image_buffer[i] = image_buffer[i] & 0xFE;

        //Put one one bit from what we got
        image_buffer[i] = image_buffer[i] | bit;
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
   int i;
    int bit;
    for(i = 0;i<32;i++)
    {
        //Get one one bit from data
        bit = ((size >> (31-i))&1);
        
        //Clear one one bit from data
        imageBuffer[i] = imageBuffer[i] & 0xFE;

        //Put one one bit from what we got
        imageBuffer[i] = imageBuffer[i] | bit;
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{

    //step 1 -> check open_files(encInfo) is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step
   if(open_files(encInfo) == e_failure)
    {
        printf("ERROR\n");
        return e_failure;
    }
    else
    {
        printf("Files opened successfully\n");
    }
      //step 2 -> check check_capacity(encInfo) is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step

       if(check_capacity(encInfo) == e_failure)
       {
        printf("ERROR\n");
        return e_failure;
       }
       else
       {
        printf("Capacity is sufficient\n");
       }
        //step 3 -> call copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step
        if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
        {
            printf("ERROR\n");
            return e_failure;
        }
        else
        {
          printf("BMP Header copied successfully\n");
        }
        //step 4 -> call encode_magic_string(MAGIC_STRING, encInfo)
        if(encode_magic_string(MAGIC_STRING, encInfo)==e_failure)
        {
            printf("ERROR\n");
            return e_failure;
        }
        else
        {
            printf("Magic string encoded\n");
        }
        //step 5 -> call encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo)
        if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo)==e_failure)
        {
            printf("ERROR\n");
            return e_failure;
        }
        else
        {
            printf("Encoded secret file extension size\n");
        }
        //step 6 -> call encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)
         if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)==e_failure)
         {
            printf("ERROR\n");
            return e_failure;
         }
         else
         {
            printf("Encoded secret file extension\n");
         }
         //step 7 -> call encode_secret_file_size(encInfo ->  size_secret_file, encInfo)
         if(encode_secret_file_size(encInfo ->  size_secret_file, encInfo)==e_failure)
         {
            printf("ERROR\n");
            return e_failure;
         }
         else
         {
            printf("Encoded secret file size\n");
         }
         //step 8 -> call encode_secret_file_data(encInfo)
         if(encode_secret_file_data(encInfo)==e_failure)
         {
            printf("ERROR\n");
            return e_failure;
         }
         else
         {
            printf("Encoded secret file data\n");
         }
         //step 9 -> copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success
            if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
            {
                printf("ERROR\n");
            }
            else
            {
                printf("Data Encoded\n");
            }
            return e_success;
        }
    
