#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include "config.h"
#include "performConnection.h"
#include "printBoard.h"
#include "sharedMemory.h"
#include "sysprakclient.h"
#include "thinker.h"


#define GAMEIDLEN 14
#define CONFFILENAMELEN 128

/* Create pipe */
int fd[2];
int * parent_shmid_ptr;

// TODO: Errormessage when no command line arguments are passed
// TODO: General Error handling

int main(int argc, char*argv[]) {

    /* Initialize user parameters */
    char game_id[GAMEIDLEN] = {0};
    int player = -1;

    /* Configuration with default to client.conf,
       but will be overriden if put in command line 
       using the flag -c */
    char conf_file[CONFFILENAMELEN] = "client.conf";

    // TODO: do we want GAME_ID and PLAYER to move to the .conf file??

    /* Read Parameters from Commandline */
    int ret;
    while ((ret = getopt(argc, argv, "g:p:c:")) != -1) {
        switch (ret) {
            case 'g':
                strncpy(game_id, optarg, 13);
                break;
            case 'p':
                player = atoi(optarg);
                break;
            case 'c':
                memset(conf_file, 0, 128);
                strncpy(conf_file, optarg, strlen(optarg) + 1);
                break;
            default:
                break;
        }
    }

    /* Get configuration details */
    configuration game_conf = read_conf_file(conf_file);

    /* Connect to Game Server */
    int socket_file_descriptor;
    socket_file_descriptor = getSocketInfo(argc, argv, game_conf.hostname, game_conf.portnumber);

    /* Create a shared memory to store the shmid of our other shared memory */
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0644);



    /*Wir erstellen eine Pipe*/
    if (pipe(fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);}


        /* Divide into Connector and Thinker */
        pid_t pid = fork();

        /* Configure signal handling */
        signal(SIGUSR1, sighandler);

        if (pid == 0) { /* Connecter (Child Process) */
            close(fd[1]); // Schreibseite schließen
            int *child_shmid_ptr;
            child_shmid_ptr = (int *) shmat(shmid, NULL, 0);

            printf("Child process ->  PPID: %d, PID: %d\n", getppid(), getpid());

            /* Enter Prolog Phase */
            serverConnect(socket_file_descriptor, game_id, player, child_shmid_ptr);

        } else { /* Thinker (Parent Process) */
           close(fd[0]); //Leseseite schließen



            // shmid_ptr is just used to store the shmid of the actual shared memory segment
            parent_shmid_ptr = (int *) shmat(shmid, NULL, 0);

            printf("Parent process -> PID: %d\n", getpid());
            printf("Waiting for child processes to finish...\n");

            // pause();

            wait(NULL);

            /* clear the shared memory segments */
            shmctl(shmid, IPC_RMID, NULL);
            shmctl(shmid_for_info, IPC_RMID, NULL);
        }

        exit(EXIT_SUCCESS);
    }


/* This function connects the client to the server of LMU
   NOTE: to work you must be connected using either a VPN
   or an SSH Tunnel */
    int getSocketInfo(int argc, char *argv[], char hostname[], char portnumber[]) {

        struct addrinfo hints;
        struct addrinfo *result, *rp;
        int socket_file_descriptor, s;

        if (argc < 3) {
            fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        /* Obtain address(es) matching host/port */
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;     /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
        hints.ai_flags = 0;
        hints.ai_protocol = 0;           /* Any protocol */

        s = getaddrinfo(hostname, portnumber, &hints, &result);
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

/* Maps signals to handler functions */
    void sighandler(int signalkey) {
        printf("Caught signal %d\n", signalkey);
        switch (signalkey) {
            case SIGUSR1:
                think(parent_shmid_ptr);
                break;
        }
    }