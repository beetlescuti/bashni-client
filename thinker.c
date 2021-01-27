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
#include <signal.h>
#include <limits.h>

#include "thinker.h"
#include "sysprakclient.h"
#include "sharedMemory.h"
#include "printBoard.h"
#include "performConnection.h"
#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMLEFT 2
#define BOTTOMRIGHT 3

int shmid_for_info;
char move[MSGLEN];
int n;

char toppiece(char board[8][8][25], int x, int y){
    return board[x][y][strlen(board[x][y])-1];
}

// upper left corner = 0
// upper right corner = 1
// lower left corner = 2
// lower right corner = 3


char** possiblemoves(char board[8][8][25], int playernumber) {
    char** all_possible_moves;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++){
            if (playernumber == 0) {
                if (toppiece(board, x, y) == 'W'){
                    for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                        possibletowermoves(board, x,y, direction);
                    }
                }
                else if (toppiece(board, x, y) == 'w'){
                    for(int direction=TOPLEFT; direction<=TOPRIGHT; direction++){
                        possibletowermoves(board, x,y, direction);
                    }
                }
            }
            else if (playernumber == 1) {
                if (toppiece(board, x, y) == 'B'){
                    for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                        possibletowermoves(board, x,y, direction);
                    }
                }
                else if (toppiece(board, x, y) == 'b'){
                    for(int direction=BOTTOMLEFT; direction<=BOTTOMRIGHT; direction++){
                        possibletowermoves(board, x,y, direction);
                    }
                }
            }
        }
    }
    return all_possible_moves;
}

char ** possibletowermoves(char board[8][8][25], int x, int y, int direction){
    char** all_possible_tower_moves;
    return all_possible_tower_moves;
}

void think(int * shmid_ptr) {
    // grab the actual shmid from shmid_ptr
    shmid_for_info = *shmid_ptr;

    // attach to shared memory
    all_info * rcv_info;
    rcv_info = (all_info*) shmat(shmid_for_info, NULL, 0);
    if (rcv_info == (void *) -1) {
    printf("Error attaching shared memory.\n");
        perror("shmat");
    }

    //check if think_flag was set by the connector process
    if (rcv_info->game_info.think_flag == 1) {
        printf("thinker_flag == %d\n", rcv_info->game_info.think_flag);
        //immediately set think_flag back to zero
        rcv_info->game_info.think_flag = 0;
        // sind wir hell oder dunkel?

        snprintf(move, strlen("A3:B4\n")+1, "A3:B4\n");
        if ((write (fd[1], move, strlen(move))) == -1) {
            perror ("write");
            exit (EXIT_FAILURE);
        }


    }
    else {
        printf("error: thinker_flag not set.\n");
    }

    // TODO: implement move thinker
    printf("thinking...\n");


    // TODO: send move back through pipe


    // detach from shared memory
    shmdt(shmid_ptr);
    shmdt(rcv_info);
}
