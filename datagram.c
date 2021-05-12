#include "datagram.h"
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h> // 
#include <unistd.h>


/**
 * datagram -> buffer
*/
void datagram_encode(datagram* dtg, char* buf)
{
    size_t len = 0;
    // first byte : code

    uint8_t code = dtg->code;
	memcpy(buf +len, &code, 1);
	len +=1;

    // second byte : type_info
    uint8_t type_info = dtg->type_info;
    memcpy(buf + len, &type_info,1);		
    len +=1;

    // third byte : id
    uint8_t id = dtg->id;
    memcpy(buf + len, &id,1);		
    len +=1;

    // payload
    memcpy(buf + len, dtg->payload,4);
}

/**
 * data -> datagram
*/
void datagram_decode(const char* data, datagram* dtg)
{
    int ptr=0; //pointeur servant à parcourir data byte par byte
   
    // first byte : code
    dtg->code = *(data+ptr);
	ptr += 1;
	
    // second byte : type_info
	dtg->type_info = *(data+ptr);
    ptr += 1;
    
    // third byte : id
	dtg->id = *(data+ptr);
    ptr += 1;
    
    // payload 
	memcpy(dtg->payload, data+ptr, 4);
}

void print_dtg(datagram* dtg_received)
{
    printf("PRINT DATAGRAM\n");
    printf("%d\n", dtg_received->code);
    printf("%d\n", dtg_received->type_info);
    printf("%d\n", dtg_received->id);
    printf("%s\n", dtg_received->payload);
}