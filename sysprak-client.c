//
//  sysprak-client.c
//  
//
//  Created by Soren Little on 12/9/20.
//

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<netdb.h>

#define GAMEKINDNAME "Baschni"
#define PORTNUMBER "1357"
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"

int main(int argc, char*argv[]) {
    
    // Initialize user parameters
    char game_id[14] = {0};
    int player;
    player = -1;
    
    // Read in from commandline
    int ret;
    while ((ret=getopt(argc, argv, "g:p:")) != -1) {
      switch (ret) {
        case 'g':
           strncpy(game_id, optarg, 13);
           break;
        case 'p':
           player = atoi(optarg);
           break;
        default:
           printf("you forgot the variables...\n");
           break;
      }
    }
    
//    // Prepare Socket
//    struct sockaddr_in address;
//    int sock = socket(PF_INET, SOCK_STREAM, 0);
//    int new_socket;
//    address.sin_family = AF_INET;
//    address.sin_port = htons(PORTNUMBER);
//    address.sin_addr = HOSTNAME;
    
//    if(connect(sock,(struct sockaddr*) &address, sizeof(address)) == 0) {
//      printf("Verbindung mit %s hergestellt.\n",inet_ntoa(address.sin_addr));
//    }
    
    
    
    
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;           /* Any protocol */

    s = getaddrinfo(HOSTNAME, PORTNUMBER, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully connect(2).
      If socket(2) (or connect(2)) fails, we (close the socket
      and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

        close(sfd);
    }

    freeaddrinfo(result);           /* No longer needed */

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }
    
}

