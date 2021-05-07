#include "datagram.h"


void datagram_encode(datagram* toEncode, char* message)
{
    message = malloc(2 + toEncode->size);
    memcpy(message, (toEncode->code<<4) | (toEncode->type_info), 1); 
    //encode premier octet
    memcpy(message+1, (toEncode->id<<4) | (toEncode->size), 1); 
    memcpy(message+2, toEncode->payload, toEncode->size);
    return;
}


char* datagram_decode(datagram* received)
{
    if(received->code ==1 )
    // temperature answer
    {
        datagram_temperature_response()
    }

    free(received);
}




// create temperature datagram;
datagram* datagram_temperature_response(float temperature)
{
    datagram_encode(1,temperature,è89,"la tempereateure  : çàiç");
    return rslt;
}
