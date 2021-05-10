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

// previously called tcp_ip_handler
static void read_data(void)
{
    memset(buf, 0, MAX_PAYLOAD_LEN);
    if(uip_newdata()) 
    {
        len = uip_datalen();
        memcpy(buf, uip_appdata, len);

        PRINTF("%u bytes from [", len);
        PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
        PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
        PRINTF("QUERY ASKED : %s", buf);

    }
    return;
}


static datagram* create_temperature_response(float temperature)
{
    char* payload = malloc(sizeof(float));
    sprintf(payload, "%.2f", temperature);

    datagram* d = malloc(sizeof(datagram));
    d->code = 0;                // code pour une reponse
    d->type_info = 0;           // pour une temperature
    d->id = 0;                  // capteur de temperature numéro 0
    d->size = sizeof(float);    // taille du payload: uniquement la température
    d->payload = payload;

    return d;

}

static void send_data_from_mote_to_server()
{
    float temperature = 17 + ((float)rand()/(float)(RAND_MAX)) * 7;  
    // random temperature between 17 and 24° [C]
    datagram* dtg = create_temperature_response(temperature);
    int length = sizeof(datagram) + dtg->size;
    char message[length];
    datagram_encode(dtg, message);

    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
    server_conn->rport = UIP_UDP_BUF->srcport;

    uip_udp_packet_send(server_conn, message, length);


    PRINTF("%u bytes sent to [", length);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
    PRINTF("ANSWER : %s", message);

    free(dtg->payload);
    free(dtg);

    // Restore server connection to allow data from any node 
    uip_create_unspecified(&server_conn->ripaddr);
    server_conn->rport = 0;    
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
    PROCESS_BEGIN();
    PRINTF("Starting the temperature sensor\n");
    
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

// modifier la simu
// envoyer un message avec le terminal 
// regarder ce qui est afficher dans le terminal toutes les 3 secondes