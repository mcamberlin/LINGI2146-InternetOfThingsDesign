// ===========================================================================
//      Temperature sensor:
// 
// -> answer to the server the current temperature when asked
// ===========================================================================


#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <stdlib.h>
#include<stdio.h>
#include "datagram.h"

#ifndef DEBUG
#define DEBUG DEBUG_FULL
#endif

#define USE_RPL_CLASSIC 0
#define SERVER_REPLY 0
#define SERVER_RPL_ROOT 0
#define MAX_PAYLOAD_LEN 120 // en Bytes

// NE PAS DEPLACER LES INCLUDES... DEMANDE A BEN AHOUM PQ...

#include "dev/leds.h"
#include "net/ipv6/uip-debug.h"
#include "dev/watchdog.h"


#if USE_RPL_CLASSIC
    #include "net/routing/rpl-classic/rpl-dag-root.h"
#else
    #include "net/routing/rpl-lite/rpl-dag-root.h"
#endif

#if SERVER_RPL_ROOT
    static uip_ipaddr_t ipaddr;
#endif


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

/*---------------------------------------------------------------------------*/

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *server_conn;
static char buf[MAX_PAYLOAD_LEN];
static uint16_t len;

static datagram dtg;
static char payload[5];

// previously called tcp_ip_handler
static void read_data(void)
{
    memset(buf, 0, MAX_PAYLOAD_LEN);
    if(uip_newdata()) 
    {
        len = uip_datalen();
        memcpy(buf, uip_appdata, len);
        PRINTF("===================================================\n");
        PRINTF("%u bytes from [", len); PRINT6ADDR(&UIP_IP_BUF->srcipaddr); PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
        
        datagram_decode(buf, &dtg);

        PRINTF("code : %d\n", dtg.code);
        PRINTF("type_info : %d\n",dtg.type_info);
        PRINTF("id : %d\n",dtg.id);;
        //PRINTF("payload : %s\n",dtg.payload);

        PRINTF("===================================================\n");

    }
    return;
}

/*
void create_temperature_response(int temperature)
{
    dtg_to_send.code = 0;                // code pour une reponse
    dtg_to_send.type_info = 0;           // pour un float
    dtg_to_send.id = 0;                  // capteur de temperature numéro 0
    sprintf( dtg_to_send.payload, "%d", temperature);
}

static void send_data_from_mote_to_server()
{
    int temperature = 17 + rand()%7;  
    // random temperature between 17 and 24° [C]
    create_temperature_response(temperature);

    int length = sizeof(dtg_to_send);
    // char message[length];
    // datagram_encode(dtg_to_send, message);

    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
    server_conn->rport = UIP_UDP_BUF->srcport;

    uip_udp_packet_send(server_conn, (char*) &dtg_to_send, length); //envoie dtg_to_send car taille fixe 

    PRINTF("===================================================\n");
    PRINTF("%u bytes sent to [", length);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
    PRINTF("ANSWER : %s", dtg_to_send.payload);
    PRINTF("===================================================\n");


    //free(dtg_to_send->payload);
    //free(dtg_to_send);

    // Restore server connection to allow data from any node 
    uip_create_unspecified(&server_conn->ripaddr);
    server_conn->rport = 0;    
}
*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
    PROCESS_BEGIN();
    
    server_conn = udp_new(NULL, UIP_HTONS(0), NULL);
    udp_bind(server_conn, UIP_HTONS(3000));

    PRINTF("Listen port: 3000, TTL=%u\n", server_conn->ttl);

    while(1) 
    {
        PROCESS_YIELD();
        
        if(ev == tcpip_event) 
        {
            PRINTF("========================================================================= \n\n");
            read_data();
            PRINTF("\n========================================================================= \n\n");
            //send_data_from_mote_to_server();
            PRINTF("\n========================================================================= \n\n");
        }
    }

    PROCESS_END();
}