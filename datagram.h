#ifndef _FEM_H_

#define _FEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <assert.h>

#define False 0 
#define True  1



typedef struct 
{
    u_int8_t code:4 ;       // query or answer ?
    u_int8_t type_info:4;   // type of information in the payload: temperature information, door information,...
    u_int8_t id:4;          // ID of a specific sensors 
    u_int8_t size:4;       // size du payload in Byte
    int payload;           // payload de 4 bytes
} datagram;


void datagram_encode(datagram* toEncode, char* message);

char* datagram_decode(datagram* received);

#endif
