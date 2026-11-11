//Name : Atluri Pranav Karthik
//Project : STEGANOGRAPHY
//Date of completion : 01-09-2026
//Date of submission : 07-09-2026
#ifndef TYPES_H
#define TYPES_H

// User defined types 
typedef unsigned int uint;

// Status will be used in fn. return type 
typedef enum
{
    e_failure,
    e_success
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
