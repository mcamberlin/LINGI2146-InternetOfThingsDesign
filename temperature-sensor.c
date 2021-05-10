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

/*---------------------------------------------------------------------------*/

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *server_conn;
static char buf[MAX_PAYLOAD_LEN];
static uint16_t len;

static datagram dtg_to_send;           // datagram used as a buffer to send to the server
static datagram received_dtg;          // datagram used as a buffer to receive and interpret the message from the sever

// previously called tcp_ip_handler
static void read_data(void)
{
    memset(buf, 0, MAX_PAYLOAD_LEN);
    if(uip_newdata()) 
    {
        len = uip_datalen();
        memcpy((void*) &received_dtg, uip_appdata, len);
        PRINTF("===================================================\n");
        PRINTF("%u bytes from [", len);
        PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
        PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
        PRINTF("%s", (char*) &received_dtg);
        
        char code = ( (char*) &received_dtg) [0] ;
        char type_info = ( (char*) &received_dtg) [1];
        char id = ( (char*) &received_dtg) [2];       

        PRINTF("QUERY ASKED : \n");
        PRINTF("\t code : %c \n", code);
        PRINTF("\t type_info : %c\n", type_info);
        PRINTF("\t id : %c\n", id); 
        PRINTF("\t payload : %s\n", received_dtg.payload);
        PRINTF("===================================================\n");

    }
    return;
}


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
            send_data_from_mote_to_server();
            PRINTF("\n========================================================================= \n\n");
        }
    }

    PROCESS_END();
}