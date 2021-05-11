#include "datagram.h"
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h> 

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
    dtg->payload = (char*) calloc(sizeof(char), 4);
	memcpy(dtg->payload, data+ptr, 4);
}

void free_datagram(datagram* dtg)
{
    free(dtg->payload);
    free(dtg);
}

int main()
{
    // test encode et decode

    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;
    dtg->type_info = 0;
    dtg->id = 9;
    dtg->payload = calloc(1,sizeof(char)*4);
    sprintf(dtg->payload,"1234");

    // encoding
    char* buf = calloc(1,sizeof(datagram));
    datagram_encode(dtg, buf);

    datagram* rcvd = calloc(1,sizeof(datagram));
    datagram_decode(buf, rcvd);

    // is equal ?
    printf("%d - %d\n",     dtg->code,           rcvd->code);
    printf("%d - %d\n",     dtg->type_info,      rcvd->type_info);
    printf("%d - %d\n",     dtg->id,             rcvd->id);
    printf("%s - %s \n",    dtg->payload,        rcvd->payload);

    exit(EXIT_SUCCESS);
    return 1;
}

//gcc -o test datagram.c && ./test