// ===========================================================================
//      Door :
// 
// -> change state of the door
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


#define PORT_NUMBER_TO_RESPOND 3023

static int state = 0;

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
	memcpy(dtg->payload, data+ptr, 4);
}
/*---------------------------------------------------------------------------*/

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *server_conn;
static uint16_t len;

static datagram dtg;
static char buf[sizeof(datagram)];
static int current_id;

static datagram response;
static char message[sizeof(datagram)];


static void read_data(void)
{
    memset(buf, 0, sizeof(datagram));

    if(uip_newdata()) 
    {
        len = uip_datalen();
        memcpy(buf, uip_appdata, len);
        PRINTF("===================================================\n");
        PRINTF("%u bytes received from server [", len); PRINT6ADDR(&UIP_IP_BUF->srcipaddr); PRINTF("]:%u, %u ... %d, %d\n", UIP_HTONS(UIP_UDP_BUF->srcport), UIP_UDP_BUF->srcport,UIP_HTONS(UIP_UDP_BUF->srcport) ,UIP_UDP_BUF->srcport);
        
        datagram_decode(buf, &dtg);

        current_id = dtg.id;
        PRINTF("code : %d\n", dtg.code);
        PRINTF("type_info : %d\n",dtg.type_info);
        PRINTF("id : %d\n",dtg.id);
        PRINTF("payload : %s\n",dtg.payload);
        PRINTF("===================================================\n");

    }
    return;
}

static void response_from_door_sensor()
{
    // create a datagram response
    response.code = 2;             // reponse code
    response.type_info = 4;        // type_info door 
    response.id = current_id;      // id of sensor

    state = !state;
    sprintf(response.payload, "%d", state);

    datagram_encode(&response,message);
    
    uint16_t rport = UIP_HTONS(PORT_NUMBER_TO_RESPOND);
    uint16_t lport =  UIP_HTONS(3000);
    server_conn->rport = rport;
    server_conn->lport = lport;
    uip_ipaddr_t serverAddr;
    uip_ip6addr(& serverAddr,0xfd00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0001);
    server_conn->ripaddr = serverAddr;

    uip_udp_packet_send(server_conn, message, 7); 

    PRINTF("===================================================\n");
    PRINTF("%u bytes answered to [", sizeof(datagram)); PRINT6ADDR(&UIP_IP_BUF->srcipaddr); PRINTF("]:%u\n", UIP_HTONS(rport));
    PRINTF("ANSWER response_from_door_sensor\n");
    PRINTF("code : %d\n", response.code);
    PRINTF("type_info : %d\n",response.type_info);
    PRINTF("id : %d\n",response.id);
    PRINTF("payload : %s\n",response.payload);
    PRINTF("===================================================\n");

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
            response_from_door_sensor();
            PRINTF("\n========================================================================= \n\n");
        }
    }
    PROCESS_END();
}
