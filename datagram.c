#include "datagram.h"

static int headerLen = 2; //in Bytes

void datagram_encode(datagram* toEncode, char* message)
{
    message = malloc(headerLen + toEncode->size);
    memcpy(message, (toEncode->code<<4) | (toEncode->type_info), 1); // encode first byte
    memcpy(message+1, (toEncode->id<<4) | (toEncode->size), 1);      // encode second byte
    memcpy(message+2, toEncode->payload, toEncode->size);            // encode payload
    return;
}

/*
char* datagram_decode(datagram* received)
{
    if(received->code ==1 )
    // temperature answer
    {
        datagram_temperature_response();
    }
    free(received);
}
*/