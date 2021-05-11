#include "datagram.h"
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h> // pour connect() 
#include <sys/socket.h> // pour getaddrinfo()
#include <netdb.h> // pour getaddrinfo()
#include <string.h>  // Pour memset()
#include <poll.h> // pour pollfd
#include <unistd.h> // pour read(), write() et close()
#include <errno.h> // pour le detail des erreurs


char addressTemperatureSensor[] = "bbbb::c30c:0:0:3";
char addressLampSensor[] = "bbbb::204:4:4:4";
char addressProximitySensor[] = "bbbb::205:5:5:5";

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

void askDistance(int i)
{
    //create datagram to encode
    datagram* dtg = calloc(1,sizeof(datagram));
    dtg->code = 1;
    dtg->type_info = 4; // type_info proximity_sensor
    dtg->id = i;        // ask to the ith temperature sensor     
    sprintf(dtg->payload,"---");

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
    char* hostname = "localhost"; 
    int src_port = 3000;//???;

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

int main()
{
    printf("Start SERVER\n");

    // ====== SOCKET CREATION ================
    int sfd = connection(); // source file descriptor
    // =======================================


    int MAXSIZE = sizeof(datagram);
	char buffer_socket[MAXSIZE];

	nfds_t nfds = 1;
	struct pollfd filedescriptors[1];

	filedescriptors[0].fd = sfd;        // Surveiller le socket
	filedescriptors[0].events = POLLIN; // Surveiller lorsque il y a de la donnee a lire sur le socket
	
	int timeout = 0; //poll() shall wait for an event to occur 

	while(1)
	{
		int p = poll(filedescriptors,nfds, timeout); 
		// int poll(struct pollfd fds[], nfds_t nfds, int timeout);
		if(p == -1)
		{
			fprintf(stderr, "Erreur dans poll read_write_loop(): %s \n", strerror(errno));
			return -1;
		}


        // ====== NEW DATAGRAM RECEIVED ================
		if(filedescriptors[0].revents & POLLIN) 
        // If there is data to read
		{			
            //1. Lire le socket
            memset(buffer_socket,0,MAXSIZE); // Remettre le buffer à 0 avant d'écrire dedans
            ssize_t r_socket = read(sfd, buffer_socket, MAXSIZE); 
            //ssize_t read(int fd, void *buf, size_t count)
            if(r_socket == -1)
            {
                fprintf(stderr, "Erreur lecture socket : %s \n", strerror(errno));
                return -1;
            }
            
            // 2. TRANSFORMER le buffer en un paquet avec decode
            datagram* dtg_received = calloc(1,sizeof(datagram));
            datagram_decode( (char*) buffer_socket, dtg_received);

            // 3. Datagram recu
            print_dtg(dtg_received);
		}
        else 
        {
            //askDistance(1);
            sleep(1);
            askTemperature(1);
            sleep(1);
            /*
            askLampState(1);
            sleep(1);
            changeLampState(1);
            sleep(1);
            */
        }
    }
    exit(EXIT_SUCCESS);
    return 1;
}
// gcc -Wall -Werror -o server datagram.h datagram.c server.c && ./server > server.txt