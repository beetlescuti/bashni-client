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
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "performConnection.h"
#include "StructDefinitions.h"

#define GAMEKINDNAME "Baschni"
#define PORTNUMBER "1357"
#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"


// TODO: Errormessage when no command line arguments are passed
// TODO: General Error handling
int getSocketInfo(int argc, char*argv[]);

int main(int argc, char*argv[]) {

    /* Initialize user parameters */
    char game_id[14] = {0};
    int player;
    player = -1;

    /* Read Parameters from Commandline */
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

    /* Connect to Game Server */
    int socket_file_descriptor;
    socket_file_descriptor = getSocketInfo(argc, argv);

    /* Create Shared Memory for GameInfo for now*/
    int shm_id = shmget(IPC_PRIVATE, sizeof(GAME_INFO), IPC_CREAT | 0666); // not sure what these flags mean IPC_CREAT | 0666
    GAME_INFO* ptrtosharedmemory;
    if (shm_id>= 0) {
        ptrtosharedmemory = shmat(shm_id, 0, IPC_CREAT | 0666);
        if (ptrtosharedmemory==(GAME_INFO *)-1) {
            perror("shmat");
        } else { /* TODO: wie bekommen wir jetzt das struct in unser shared memory?*/
            /* Write GAME_INFO to ptr */
             //   ptrtosharedmemory[0] = GAME_INFO info_for_this_game {0,0,0, "name"};
            printf("shared memory successfully acquired but will now be deleted...\n");
            shmctl(shm_id, IPC_RMID, NULL);;
        }
    } else { /* shmget lief schief */
        perror("shmget");
    }



/* divide into connector and thinker */
    pid_t pid = fork();

    if(pid == 0) {
        printf("Child process => PPID=%d, PID=%d\n", getppid(), getpid()); // I am the connector
        /* Enter Prolog Phase */

        serverConnect(socket_file_descriptor, game_id, player);
        exit(0);
    }
    else  {
        printf("Parent process => PID=%d\n", getpid()); // I am the thinker, I do not have a job yet.
        printf("Waiting for child processes to finish...\n");
        wait(NULL);
        printf("child process finished.\n");
    }




    exit(EXIT_SUCCESS);
}


/* This function connects the client to the server of LMU
   NOTE: to work you must be connected using either a VPN
   or an SSH Tunnel */
int getSocketInfo(int argc, char*argv[]) {

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int socket_file_descriptor, s;

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
        socket_file_descriptor = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (socket_file_descriptor == -1)
            continue;

        if (connect(socket_file_descriptor, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

        close(socket_file_descriptor);
    }

    freeaddrinfo(result);           /* No longer needed */

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    return socket_file_descriptor;
}


