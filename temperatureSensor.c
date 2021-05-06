// ===========================================================================
//      Temperature sensor:
// 
// -> toutes les 3 secondes, envoie au border router la temperature (un nombre 
// aléatoire)
// ===========================================================================

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#ifndef DEBUG
#define DEBUG DEBUG_FULL
#endif

#define USE_RPL_CLASSIC 0
#define SERVER_REPLY 0
#define SERVER_RPL_ROOT 0
#define MAX_PAYLOAD_LEN 120

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
    if(uip_newdata()) {
        leds_on(LEDS_RED);
        len = uip_datalen();
        memcpy(buf, uip_appdata, len);


        PRINTF("%u bytes from [", len);
        PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
        PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
        PRINTF("CONTENT : %s", buf);


    #if SERVER_REPLY
        uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
        server_conn->rport = UIP_UDP_BUF->srcport;

        uip_udp_packet_send(server_conn, buf, len);
        /* Restore server connection to allow data from any node */
        uip_create_unspecified(&server_conn->ripaddr);
        server_conn->rport = 0;
    #endif
    }
    leds_off(LEDS_RED);
    return;
}

// Needs changes
static void send_data_from_mote_to_root()
{
    int nbCharInMessage = 16;
    uint16_t lengthMessage = sizeof(char) * nbCharInMessage;
    char message [] = "hello from mote";
    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
    server_conn->rport = UIP_UDP_BUF->srcport;

    uip_udp_packet_send(server_conn, message, lengthMessage);


    PRINTF("%u bytes sent to [", lengthMessage);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
    PRINTF("CONTENT : %s", message);

    /* Restore server connection to allow data from any node */
    uip_create_unspecified(&server_conn->ripaddr);
    server_conn->rport = 0;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
    PROCESS_BEGIN();
    PRINTF("Starting the server\n");
    
    server_conn = udp_new(NULL, UIP_HTONS(0), NULL);
    udp_bind(server_conn, UIP_HTONS(3000));

    PRINTF("Listen port: 3000, TTL=%u\n", server_conn->ttl);

    while(1) 
    {
        /*
        // every 3 secondes, send temperatures:
        PRINTF("========================================================================= \n\n");
        send_data_from_mote_to_root();
        PRINTF("\n========================================================================= \n\n");
        */

        PROCESS_YIELD();
        if(ev == tcpip_event) 
        {
            read_data();
        }
    }

    PROCESS_END();
}
