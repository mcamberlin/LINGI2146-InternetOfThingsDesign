#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#ifndef DEBUG
    #define DEBUG DEBUG_FULL
#endif

#define USE_RPL_CLASSIC 0

#include "dev/leds.h"

#include "net/ipv6/uip-debug.h"
#include "dev/watchdog.h"

#if USE_RPL_CLASSIC
    #include "net/routing/rpl-classic/rpl-dag-root.h"
#else
    #include "net/routing/rpl-lite/rpl-dag-root.h"
#endif

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;
static char buf[MAX_PAYLOAD_LEN];
static uint16_t len;

#define SERVER_REPLY 1

/*---------------------------------------------------------------------------*/

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/

static struct uip_udp_conn *server_conn;
static char buf[MAX_PAYLOAD_LEN];
static uint16_t len;

/*---------------------------------------------------------------------------*/

static void udpip_handler(void)
{
    memset(buf, 0, MAX_PAYLOAD_LEN);
    if(uip_newdata()) 
    {
        leds_on(LEDS_RED);
        len = uip_datalen();
        memcpy(buf, uip_appdata, len);
        PRINTF("%u bytes from [", len);
        PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
        PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));

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
        PROCESS_YIELD();
        if(ev == tcpip_event) 
        {
            PRINTF("WOW");
            udpip_handler();
        }
    }

    PROCESS_END();
}
