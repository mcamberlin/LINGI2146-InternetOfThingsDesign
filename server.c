#include "datagram.h"
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h> 
#include <unistd.h>

char addressTemperatureSensor[] = "bbbb::c30c:0:0:3";


void askTemperature()
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;
    dtg->type_info = 5;
    dtg->id = 9;
    dtg->payload = calloc(1,sizeof(char)*4);
    sprintf(dtg->payload,"1234");

    // encoding
    char* buf = calloc(1,sizeof(datagram) + 5);
    datagram_encode(dtg, buf);


    char query[120];
    sprintf(query,"echo %s | nc -u %s 3000 &", buf, addressTemperatureSensor);
    //printf("%s\n",query);

    system(query);
}

void server()
{
    printf("Start SERVER\n");
    while(1)
    {
        askTemperature();
        sleep(3);
    }

}


int main()
{
    // test encode et decode

    /*
    datagram* rcvd = calloc(1,sizeof(datagram));
    datagram_decode(buf, rcvd);

    // is equal ?
    printf("%d - %d\n",     dtg->code,           rcvd->code);
    printf("%d - %d\n",     dtg->type_info,      rcvd->type_info);
    printf("%d - %d\n",     dtg->id,             rcvd->id);
    printf("%s - %s \n",    dtg->payload,        rcvd->payload);
    */

    /*
    system("ping -c 1 192.168.1.1 & ");
    sleep(3);
    char buffer[123];
    read(0, &buffer, 123);
    system("echo Merlin");
    printf(" ================= \n %s\n", buffer);
    */
    server();
    exit(EXIT_SUCCESS);
    return 1;
}
