#include "datagram.h"
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h> 
#include <unistd.h>

char addressTemperatureSensor[] = "bbbb::c30c:0:0:3";
char addressLampSensor[] = "bbbb::204:4:4:4";

void askTemperature(int i)
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;
    dtg->type_info = 4; // type_info temperature
    dtg->id = i;        // ask to the ith temperature sensor     
    sprintf(dtg->payload,"---");

    // encoding
    char* buf = calloc(1,sizeof(datagram) + 4);
    datagram_encode(dtg, buf);
    free(dtg);

    char query[120];
    sprintf(query,"echo %s | nc -u %s 3000 &", buf, addressTemperatureSensor);
    //printf("%s\n",query);

    system(query);
    free(buf);
}



void askLampState(int i)
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;
    dtg->type_info = 3; // type_info temperature
    dtg->id = i;        // ask to the ith temperature sensor     
    sprintf(dtg->payload,"---");

    // encoding
    char* buf = calloc(1,sizeof(datagram) + 4);
    datagram_encode(dtg, buf);
    free(dtg);

    char query[120];
    sprintf(query,"echo %s | nc -u %s 3000 &", buf, addressLampSensor);
    //printf("%s\n",query);

    system(query);
    free(buf);
}

void changeLampState(int i)
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;
    dtg->type_info = 2; // type_info temperature
    dtg->id = i;        // ask to the ith temperature sensor     
    sprintf(dtg->payload,"---");

    // encoding
    char* buf = calloc(1,sizeof(datagram) + 4);
    datagram_encode(dtg, buf);
    free(dtg);

    char query[120];
    sprintf(query,"echo %s | nc -u %s 3000 &", buf, addressLampSensor);
    //printf("%s\n",query);

    system(query);
    free(buf);
}

int main()
{
    printf("Start SERVER\n");
    while(1)
    {
        //askTemperature(1);
        //askLampState(1);
        //sleep(1);
        changeLampState(1);
        sleep(1);
    }
    exit(EXIT_SUCCESS);
    return 1;
}

/*
    system("ping -c 1 192.168.1.1 & ");
    sleep(3);
    char buffer[123];
    read(0, &buffer, 123);
    system("echo Merlin");
    printf(" ================= \n %s\n", buffer);
*/