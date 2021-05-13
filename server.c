//gcc -Wall -Werror -o server datagram.h datagram.c server.c -lpthread && ./server
#include "datagram.h"
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <string.h> 
#include <poll.h> 
#include <pthread.h>
#include <errno.h> 

#define cursup    "\033[A"
#define curshome  "\033[0;0H"

// PORT NUMBER OF THE SERVER TO LISTEN TO ANSWER OF MOTES
#define PORT_NUMBER 3023

char addressBorderRouter[] = "bbbb::c30c:0:0:1";

char addressProximitySensor[]   = "bbbb::202:2:2:2";
char addressLampSensor[]        = "bbbb::203:3:3:3";
char addressTemperatureSensor[] = "bbbb::204:4:4:4";
char addressDoorSensor[]        = "bbbb::205:5:5:5";


static char proximity [4]   = "0";
static char lampState [4]   = "0";
static char temperature [4] = "0";
static char doorState [4]   = "0";

int input=0;
int stop = 0;
pthread_mutex_t lock_data;

/* First arg of the 4 following functions is the ID of
 the sensor to request a data or to change its state */
void askDistance(int i)
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;                  // query code
    dtg->type_info = 1;             // type_info proximity_sensor
    dtg->id = i;                    // ask to the ith temperature sensor     
    sprintf(dtg->payload,"---");    // empty payload

    // encoding
    char* buf = calloc(1,sizeof(datagram) + 4);
    datagram_encode(dtg, buf);
    free(dtg);

    char query[120];
    sprintf(query,"echo %s | nc -u %s 3000 &", buf, addressProximitySensor);
    //printf("%s\n",query);

    system(query);
    free(buf);
}

void changeLampState(int i)
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;                  // query code
    dtg->type_info = 2;             // type_info lamp state 
    dtg->id = i;                    // ask to the ith lamp      
    sprintf(dtg->payload,"---");    // empty payload

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

void askTemperature(int i)
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;                  // query code
    dtg->type_info = 3;             // type_info temperature 
    dtg->id = i;                    // ask to the ith temperature sensor     
    sprintf(dtg->payload,"---");    // empty payload  

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

void changeDoorState(int i)
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;                  // query code
    dtg->type_info = 4;             // type_info door sensor
    dtg->id = i;                    // ask to the ith door sensor     
    sprintf(dtg->payload,"---");    // empty payload

    // encoding
    char* buf = calloc(1,sizeof(datagram) + 4);
    datagram_encode(dtg, buf);
    free(dtg);

    char query[120];
    sprintf(query,"echo %s | nc -u %s 3000 &", buf, addressDoorSensor);
    //printf("%s\n",query);

    system(query);
    free(buf);
}

/** La fonction real_address() permet de convertir une chaîne de caractères représentant soit un nom de domaine soit une adresse IPv6, 
 * en une structure  struct @sockaddr_in6 utilisable par l'OS 
 * @address: The name to resolve
 * @rval: Where the resulting IPv6 address descriptor should be stored
 * @return: NULL if it succeeded, or a pointer towards
 *          a string describing the error if any.
 */
const char* real_address(const char *address, struct sockaddr_in6 *rval)
{
    // 1. Récupérer les informations concernant l'addresse
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;     // AF_INET6 indique que getaddrinfo() ne devrait retourner que des adresses IPv6.
    hints.ai_socktype = SOCK_DGRAM; // Type de socket pour les protocoles UDP 
    hints.ai_flags=AI_CANONNAME;   
    
    struct addrinfo *res;
    char *service = NULL;

    int test = getaddrinfo(address, service, &hints, &res);
    //int getaddrinfo (const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
    if (test != 0) // Si ca plante 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(test));
        return gai_strerror(test);
    } 
 
    struct sockaddr_in6* adresseIPv6 = (struct sockaddr_in6*) res->ai_addr; 
    
    // 2. Copier le résultat de getaddrinfo() dans rval 
    
    memcpy(rval,adresseIPv6,sizeof(struct sockaddr_in6));
    // void *memcpy(void *dest, const void *src, size_t n);
    
    // 3. Libérer res
    freeaddrinfo(res);

    return NULL;
}

int create_socket(struct sockaddr_in6 *source_addr, int src_port, struct sockaddr_in6 *dest_addr, int dst_port)
{

	int fd = socket(AF_INET6, SOCK_DGRAM, 0);  //int socket(int domain, int type, int protocol); 
	// AF_INET6 pour IPv6 et SOCK_DGRAM pour UDP et 0 pour protocole par default ou IPPROTO_UDP.
	if (fd == -1)
	{  
		fprintf(stderr, "Erreur creation de socket dans create_socket(): %s , errno %d\n", strerror(fd), fd);
        stop =1;
        return -1;
	}

	if(src_port > 0)
	{
		source_addr->sin6_port = htons(src_port);
	}
	if(dst_port > 0)
	{
		dest_addr->sin6_port = htons(dst_port);
	}	
	
	// 2. Lier le socket avec la source using the bind() system call
	if(source_addr != NULL)
	{                    
		int lien = bind(fd,(struct sockaddr*) source_addr, (socklen_t) sizeof(struct sockaddr_in6));
	        //int bind(int socket, const struct sockaddr *address, socklen_t address_len);
		if(lien == -1)
		{
		    //close(fd); la fermeture des fd se fera tout a la fin de receiver.c
		    fprintf(stderr,"Erreur dans bind() : %s\n", strerror(errno));
            stop = 1;
		    return -1;
		}   
	}
	
	// 3.Connect the socket to the address of the destination using the connect() system call
	if(dest_addr != NULL)
	{
		int connect_src = connect(fd,(struct sockaddr*) dest_addr, (socklen_t) sizeof(struct sockaddr_in6));
		if(connect_src == -1)
		{
		    //close(fd); la fermeture des fd se fera tout a la fin de receiver.c
		    fprintf(stderr,"Erreur dans connect() \n");
		    return -1;
		}
	}

	return fd;    
}

int connection()
{
    char* hostname = "fd00::1"; 
    int src_port = PORT_NUMBER;

    // src_addr = our machine
    struct sockaddr_in6 src_addr;	
	const char *err = real_address(hostname, &src_addr);
	if (err != NULL)
	{
		fprintf(stderr, "Could not resolve hostname %s: %s\n", hostname, err);
		return EXIT_FAILURE;
	}
   
	int sfd = create_socket( &src_addr, src_port,NULL,-1);
    // Creation d'un socket pour toutes les connexions sans le connecter pour ecouter de partout
	//int create_socket(struct sockaddr_in6 *source_addr, int src_port, struct sockaddr_in6 *dest_addr, int dst_port)
	if(sfd == -1)
	{
		fprintf(stderr,"Erreur createsocket() \n");	
	}
    return sfd;
}

const char theHelpMessage[] = { 
                                "\n-----------------------------\n"
                                " TYPE   :  ACTION \n"
                                "   1    : to update proximity \n"
                                "   2    : to update lamp state \n"
                                "   3    : to update temperature \n"
                                "   4    : to update door state\n"
                                "   5    : to exit \n"
                             };

void printProximity()
{
    printf(cursup);
    printf(cursup);
    printf(cursup);
    printf(cursup);
    printf("\r");
    printf("Proximity = %s",proximity);
    printf("\n\n\n\n");
    fflush(stdout);  //comment this, to see the difference in O/P
}

void printLamp()
{
    printf(cursup);
    printf(cursup);
    printf(cursup);
    printf("\r");
    printf("Lamp = %s",lampState);
    printf("\n\n\n");
    fflush(stdout);  //comment this, to see the difference in O/P
}

void printTemperature()
{
    printf(cursup);
    printf(cursup);
    printf("\r");
    printf("Temperature = %s",temperature);
    printf("\n\n");
    fflush(stdout);  //comment this, to see the difference in O/P
}

void printDoor()
{
    printf(cursup);
    printf("\r");
    printf("Door = %s",doorState);
    printf("\n");
    fflush(stdout);  //comment this, to see the difference in O/P
}

void PrintValues(void* ptr) {
    printf("\n======= INTERNET OF THINGS =======================\n");
    printf("%s\n", theHelpMessage);
    printf("==============================\n");
    printf("Proximity sensor: 1 \t \n");
    printf("Lamp state      : 2\n");
    printf("Temperature     : 3\n");
    printf("Door state      : 4\n");
    printf("Proximity = %s\n",proximity);
    printf("Lamp = %s\n",lampState);
    printf("Temperature = %s\n",temperature);
    printf("Door = %s\n",doorState);
}

void Input(void* ptr)
{
    sleep(1);
    while(1)
    {
        scanf("%d", &input);
        sleep(0.5);
        if(input == 5)
        {
            stop = 1;
            return;
        }
        printf("\b");
        printf(cursup);
    }
}

void server(void* ptr)
{
    stop = 0;
    int sfd = connection(); 

    int MAXSIZE = sizeof(datagram);
	char buffer_socket[MAXSIZE];

	nfds_t nfds = 1;
	struct pollfd filedescriptors[1];

	filedescriptors[0].fd = sfd;        // Surveiller le socket
	filedescriptors[0].events = POLLIN; // Surveiller lorsque il y a de la donnee a lire sur le socket
	
	int timeout = 0; //poll() shall wait for an event to occur 

	while(!stop)
	{
		int p = poll(filedescriptors,nfds, timeout); 
		// int poll(struct pollfd fds[], nfds_t nfds, int timeout);
		if(p == -1)
		{
			fprintf(stderr, "Erreur dans poll read_write_loop(): %s \n", strerror(errno));
			return;
		}


        // ====== NEW DATAGRAM RECEIVED ================
		if(filedescriptors[0].revents & POLLIN) 
        // If there is data to read
		{			
            //1. Lire le socket
            memset(buffer_socket,0,MAXSIZE); // Remettre le buffer à 0 avant d'écrire dedans
            ssize_t r_socket = read(sfd, buffer_socket, MAXSIZE); 
            if(r_socket == -1)
            {
                fprintf(stderr, "Erreur lecture socket : %s \n", strerror(errno));
                return;
            }
            
            // 2. TRANSFORMER le buffer en un paquet avec decode
            datagram* dtg_received = calloc(1,sizeof(datagram));
            datagram_decode( (char*) buffer_socket, dtg_received);

            // 3. Datagram recu => update the corresponding maintained states
            //print_dtg(dtg_received);
            if(dtg_received->type_info == 1)
            // proximity data received
            {
                pthread_mutex_lock(&lock_data);
                memcpy(proximity, dtg_received->payload,4);
                printProximity();
                if(atoi(proximity) > 20)
                {
                    changeLampState(1);
                }
                pthread_mutex_unlock(&lock_data);
                //printf("\n\t\t\t\t\t\t\t\t proximity: %d\n", atoi(proximity));

            }
            else if(dtg_received->type_info == 2)
            // lamp state received
            {
                pthread_mutex_lock(&lock_data);
                memcpy(lampState, dtg_received->payload,4);
                printLamp();
                pthread_mutex_unlock(&lock_data);
                //printf("\t\t\t\t\t\t\t\t lamp state: %s\n", lampState);

            }
            else if(dtg_received->type_info == 3)
            // temperature data received
            {
                pthread_mutex_lock(&lock_data);
                memcpy(temperature, dtg_received->payload,4);
                printTemperature();
                if(atoi(temperature) > 18)
                {
                    changeDoorState(1);
                }
                pthread_mutex_unlock(&lock_data);
                //printf("\t\t\t\t\t\t\t\t temperature: %s\n", temperature);
            }
            else if(dtg_received->type_info == 4)
            // door state received
            {
                pthread_mutex_lock(&lock_data);
                memcpy(doorState, dtg_received->payload,4);
                printDoor();
                pthread_mutex_unlock(&lock_data);
                //printf("\t\t\t\t\t\t\t\t door state: %s\n", doorState);
            }
		}
        // ====== NO DATAGRAM RECEIVED => ASK UPDATES ================
        else 
        {   
            switch (input)
            {
                case 1:
                    askDistance(1);
                    pthread_mutex_lock(&lock_data);
                    input = 0;
                    pthread_mutex_unlock(&lock_data);
                    break;
                case 2:
                    changeLampState(1);
                    pthread_mutex_lock(&lock_data);
                    input = 0;
                    pthread_mutex_unlock(&lock_data);
                    break;
                case 3:
                    askTemperature(1);
                    pthread_mutex_lock(&lock_data);
                    input = 0;
                    pthread_mutex_unlock(&lock_data);
                    break;
                case 4:
                    changeDoorState(1);
                    pthread_mutex_lock(&lock_data);
                    input = 0;
                    pthread_mutex_unlock(&lock_data);
                    break;
                case 5:
                    stop = 1;
                    break;
            }
        }
    }
    close(sfd);
    return;
}

int main(void) 
{
    if (pthread_mutex_init(&lock_data, NULL) != 0) 
    {
        printf("\n mutex init has failed\n");
        return 1;
    }
    int* ptr;
    pthread_t tid_print;
    pthread_t tid_input;
    pthread_t tid_server;
    pthread_create(&tid_print, NULL, (void*)PrintValues, 0);
    pthread_create(&tid_input, NULL, (void*)Input, 0);
    pthread_create(&tid_server, NULL, (void*)server, 0);
    pthread_join(tid_server, (void**) &ptr);
    return 0;
}