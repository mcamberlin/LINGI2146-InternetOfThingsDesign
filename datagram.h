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
    u_int8_t type_info;   // type of information in the payload: temperature information, door information,...
    u_int8_t code;        // query or answer ?
    u_int8_t id;          // ID of a specific sensors 
    char* payload;        // payload de 4 bytes
} datagram;


void datagram_encode(datagram* dtg, char* buf);

void datagram_decode(const char* data, datagram* dtg);

void free_datagram(datagram* dtg);

#endif
