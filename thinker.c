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

// 5 for first move, 11 max partial moves with 3 chars each, plus nullbyte
#define MAXMOVELEN 5 + 11*3 + 1
#define MAXMOVES 500
#define POSLEN 3

int shmid_for_info;
char move[MSGLEN-6];
int n;
int our_playernum;

char tower_move[MAXMOVELEN];
bool break_case = true;
char** calculated_moves;


void think(int * shmid_ptr) {
    printf("were in the thinker...\n");
    // grab the actual shmid from shmid_ptr
    shmid_for_info = *shmid_ptr;

    // attach to shared memory
    all_info * rcv_info;
    rcv_info = (all_info*) shmat(shmid_for_info, NULL, 0);
    if (rcv_info == (void *) -1) {
    printf("Error attaching shared memory.\n");
        perror("shmat");
    }

    // check if think_flag was set by the connector process
    if (rcv_info->game_info.think_flag == 1) {
        // immediately set think_flag back to zero
        rcv_info->game_info.think_flag = 0;
        // sind wir hell oder dunkel?

        snprintf(move, strlen("A3:B4\n")+1, "A3:B4\n");

        printboard(rcv_info->game_info.board);
        calculated_moves = possiblemoves(rcv_info->game_info.board);

        if ((write (fd[1], move, strlen(move))) == -1) {
            perror ("write");
            exit (EXIT_FAILURE);
        }


    }
    else {
        printf("error: thinker_flag not set.\n");
    }

    // set our player number
    our_playernum = rcv_info->game_info.our_playernum;

    printf("thinking...\n");

    // TODO: send move back through pipe

    // detach from shared memory
    // shmdt(shmid_ptr);
    // shmdt(rcv_info);
}

/* calculates the possible moves of all of our pieces */
char** possiblemoves(char board[8][8][25]) {

    static char all_possible_moves[MAXMOVES][MAXMOVELEN];
    int num_moves = 0;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++){
            if (our_playernum == 0) {
                if (toppiece(board, x, y) == 'W'){
                    char all_possible_tower_moves[MAXMOVES][MAXMOVELEN];
                    for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                      //TODO: create new function that calculates the moves a queen can do, since it will behave differently in calc_move()
                      //possibletowermoves(board, x,y, direction);
                    }
                }
                else if (toppiece(board, x, y) == 'w'){
                    char all_possible_tower_moves[MAXMOVES][MAXMOVELEN];
                    for(int direction=TOPLEFT; direction<=TOPRIGHT; direction++){

                        possibletowermoves(board, x, y, direction);

                        if (strcmp(tower_move, "") != 0) {
                            snprintf(all_possible_tower_moves[num_moves], MAXMOVELEN, "%s", tower_move);
                            snprintf(all_possible_moves[num_moves], MAXMOVELEN, "%s", tower_move);

                            printf("MOVE: %s\n", all_possible_moves[num_moves]);

                            num_moves++;
                        }
                    }
                }
            }
            else if (our_playernum == 1) {
                if (toppiece(board, x, y) == 'B'){
                    char all_possible_tower_moves[MAXMOVES][MAXMOVELEN];
                    for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                        //TODO: create new function that calculates the moves a queen can do, since it will behave differently in calc_move()
                        //possibletowermoves(board, x,y, direction);
                    }
                }
                else if (toppiece(board, x, y) == 'b'){
                    char all_possible_tower_moves[MAXMOVES][MAXMOVELEN];
                    for(int direction=BOTTOMLEFT; direction<=BOTTOMRIGHT; direction++){
                        possibletowermoves(board, x,y, direction);

                        if (strcmp(tower_move, "") != 0) {
                            snprintf(all_possible_tower_moves[num_moves], MAXMOVELEN, "%s", tower_move);
                            snprintf(all_possible_moves[num_moves], MAXMOVELEN, "%s", tower_move);

                            printf("MOVE: %s\n", all_possible_moves[num_moves]);

                            num_moves++;
                        }
                    }
                }
            }
        }
    }
    return (char**)all_possible_moves;
}

/* FOR NORMAL PIECES:
   calculates the possible moves of ONE pieces in ONE direction,
   this should be looped over in possiblemoves,
   returns empty string if no moves availalbe */
void possibletowermoves(char board[8][8][25], int x, int y, int direction) {
    int pos_x = x;
    int pos_y = y;

    memset(tower_move, 0, MAXMOVELEN);

    int* newpoint;
    newpoint = moveindirection(direction, pos_x, pos_y);
    pos_x = newpoint[0];
    pos_y = newpoint[1];


    
//    switch (direction) {
//        case TOPLEFT:
//            pos_x--;
//            pos_y++;
//            break;
//        case TOPRIGHT:
//            pos_x++;
//            pos_y++;
//            break;
//        case BOTTOMLEFT:
//            pos_x--;
//            pos_y--;
//            break;
//        case BOTTOMRIGHT:
//            pos_x++;
//            pos_y--;
//            break;
//        default:
//            break;
//    }

    /* catch case for edge of board */
    if (pos_x >= 0 && pos_x <= 7 && pos_y >= 0 && pos_y <= 7) {
        if (our_playernum == 0) {
            switch (toppiece(board, pos_x, pos_y)) {
                case 'b':
                    // check if field behind is open
                    // if (toppiece(board, pos_x, pos_y))
                    break;
                case 'B':
                    break;
                case ' ':
                    if (strcmp(tower_move, "") == 0) {
                        char pre_pos[POSLEN];
                        snprintf(pre_pos, POSLEN, "%s", translate_pos(x, y));
                        char post_pos[POSLEN];
                        snprintf(post_pos, POSLEN, "%s", translate_pos(pos_x, pos_y));
                        snprintf(tower_move, MAXMOVELEN, "%s:%s", pre_pos, post_pos);
                    }
                    break;
                default:
                    break;
            }
        }
        else if (our_playernum == 1) {
            // TODO: translate move logic for black
        }
    }


}

/* FOR A QUEEN PIECE:
   calculates the possible moves of ONE pieces in ONE direction,
   this should be looped over in possiblemoves,
   returns empty string if no moves availalbe */
void possibletowermoves_queen(char board[8][8][25], int x, int y, int direction){
    int pos_x = x;
    int pos_y = y;

    if (direction == TOPLEFT) {
        break_case = true;
        memset(tower_move, 0, MAXMOVELEN);
        while (pos_x >= 0 && pos_y <= 7 && break_case) {
            pos_x--;
            pos_y++;

            //calc_move(board, x, y, pos_x, pos_y);
        }
    }
    else if (direction == TOPRIGHT) {
        break_case = true;
        memset(tower_move, 0, MAXMOVELEN);
        while (pos_x <= 7 && pos_y <= 7 && break_case) {
            pos_x++;
            pos_y++;

            //calc_move(board, x, y, pos_x, pos_y);
        }
    }
    else if (direction == BOTTOMLEFT) {
        while (pos_x >= 0 && pos_y >= 0 && break_case) {
            pos_x--;
            pos_y--;

            //calc_move(board, x, y, pos_x, pos_y);
        }
    }
    else if (direction == BOTTOMRIGHT) {
        while (pos_x <= 7 && pos_y >= 0 && break_case) {
            pos_x++;
            pos_y--;

            //calc_move(board, x, y, pos_x, pos_y);
        }
    }
}

/* returns the value of the top piece from board coordinates */
char toppiece(char board[8][8][25], int x, int y){
    char piece = ' ';
    if (board[x][y][strlen(board[x][y])-1]) {
        piece = board[x][y][strlen(board[x][y])-1];
    }
    return piece;
}

/* translates an x and y coord into a protocoll readable format
   for example: 0, 0 becomes A1 */
char* translate_pos(int x, int y) {
    static char position[POSLEN];
    memset(position, 0, POSLEN);

    switch (x) {
        case 0:
            position[0] = 'A';
            break;
        case 1:
            position[0] = 'B';
            break;
        case 2:
            position[0] = 'C';
            break;
        case 3:
            position[0] = 'D';
            break;
        case 4:
            position[0] = 'E';
            break;
        case 5:
            position[0] = 'F';
            break;
        case 6:
            position[0] = 'G';
            break;
        case 7:
            position[0] = 'H';
            break;
        default:
            printf("Error: how did we get here...\n");
            break;
    }

    // the + '0' converts the int to a char with the same value
    position[1] = y+1 + '0';

    return position;
}


int* moveindirection(int direction, int x, int y) {
       static int newpoint[2];
       newpoint[0] = x;
       newpoint[1] = y;

        switch (direction) {
            case TOPLEFT:
                newpoint[0]--;
                newpoint[1]++;
                break;
            case TOPRIGHT:
                newpoint[0]++;
                newpoint[1]++;
                break;
            case BOTTOMLEFT:
                newpoint[0]--;
                newpoint[1]--;
                break;
            case BOTTOMRIGHT:
                newpoint[0]++;
                newpoint[1]--;
                break;
            default:
                break;
        }
        
        return newpoint;
}