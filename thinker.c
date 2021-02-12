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

#define BADMOVE 1
#define BETTERMOVE 2
#define EVENBETTERMOVE 3

// 5 for first move, 11 max partial moves with 3 chars each, plus nullbyte
#define MAXMOVELEN 5 + 11*3 + 1
#define MAXMOVES 500
#define POSLEN 3

int shmid_for_info;
char move[MSGLEN-6];
int n;
int our_playernum;
int first_think = 0;

char tower_move[MAXMOVELEN];
bool break_case = true;
char** calculated_moves;

char all_possible_moves[MAXMOVES][MAXMOVELEN];
int flag_all_possible_moves[MAXMOVES];
int num_moves = 0;

all_info * rcv_info = NULL;

void think(int * shmid_ptr) {
    printf("were in the thinker...\n");
    // grab the actual shmid from shmid_ptr
    shmid_for_info = *shmid_ptr;

    // attach to shared memory
    if (first_think == 0) {
        rcv_info = (all_info*) shmat(shmid_for_info, NULL, 0);
        if (rcv_info == (void *) -1) {
            printf("Error attaching shared memory.\n");
        perror("shmat");
        }
        first_think = 1;
    }
    

    // set our player number
    our_playernum = rcv_info->game_info.our_playernum;



    // check if think_flag was set by the connector process
    if (rcv_info->game_info.think_flag == 1) {
        // immediately set think_flag back to zero
        rcv_info->game_info.think_flag = 0;
        // sind wir hell oder dunkel?


        printboard(rcv_info->game_info.board);
        calculated_moves = possiblemoves(rcv_info->game_info.board);
        int selected_move = bestmove();
        printf("SELECTED MOVE: %d\n", selected_move);

        snprintf(move, strlen(all_possible_moves[selected_move]) + 2, "%s\n", all_possible_moves[selected_move]);

        //reset all_possible_moves, all_possible_tower_moves, flag_all_possible_moves
        for (size_t i = 0; i < MAXMOVES; i++) {
            memset(all_possible_moves[i], 0, MAXMOVELEN);
            flag_all_possible_moves[i] = 0;
            //memset(all_possible_tower_moves[i], 0, MAXMOVELEN);
        }

        //write move into pipe
        if ((write (fd[1], move, strlen(move) + 1)) == -1) {
            perror ("write");
            exit (EXIT_FAILURE);
        }
    }

    else {
        printf("error: thinker_flag not set.\n");
    }

    // detach from shared memory
    // shmdt(shmid_ptr);
    // shmdt(rcv_info);
}

/* takes an array of moves and selects the best one */
int bestmove() {
    int best_index = -1;
    int best_rating = 0;
    for (int i = 0; i < MAXMOVES; i++) {
        if (flag_all_possible_moves[i] > best_rating) {
            best_rating = flag_all_possible_moves[i];
            best_index = i;
        }
    }
    return best_index;
}

/* calculates the possible moves of all of our pieces */
char** possiblemoves(char board[8][8][25]) {
    num_moves = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++){
            if (our_playernum == 0) {
                if (toppiece(board, x, y) == 'W'){
                    // char all_possible_tower_moves[MAXMOVES][MAXMOVELEN];
                    for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                      //TODO: create new function that calculates the moves a queen can do, since it will behave differently in calc_move()
                      //possibletowermoves(board, x,y, direction);
                    }
                }
                else if (toppiece(board, x, y) == 'w'){
                    // char all_possible_tower_moves[MAXMOVES][MAXMOVELEN];
                    for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                        
                        memset(tower_move, 0, MAXMOVELEN);
                        possibletowermoves(board, x, y, direction);

                        if (strcmp(tower_move, "") != 0) {
                            // snprintf(all_possible_tower_moves[num_moves], MAXMOVELEN, "%s", tower_move);
                            snprintf(all_possible_moves[num_moves], MAXMOVELEN, "%s", tower_move);

                            printf("MOVE: %s %d\n", all_possible_moves[num_moves], flag_all_possible_moves[num_moves]);

                            num_moves++;
                        }
                    }
                }
            }
            else if (our_playernum == 1) {
                if (toppiece(board, x, y) == 'B'){
                    // char all_possible_tower_moves[MAXMOVES][MAXMOVELEN];
                    for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                        //TODO: create new function that calculates the moves a queen can do, since it will behave differently in calc_move()
                        //possibletowermoves(board, x,y, direction);
                    }
                }
                else if (toppiece(board, x, y) == 'b'){
                    // char all_possible_tower_moves[MAXMOVES][MAXMOVELEN];
                    for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                        possibletowermoves(board, x,y, direction);

                        if (strcmp(tower_move, "") != 0) {
                            // snprintf(all_possible_tower_moves[num_moves], MAXMOVELEN, "%s", tower_move);
                            snprintf(all_possible_moves[num_moves], MAXMOVELEN, "%s", tower_move);

                            printf("MOVE: %s %d\n", all_possible_moves[num_moves], flag_all_possible_moves[num_moves]);

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

    int* newpoint;
    newpoint = moveindirection(direction, pos_x, pos_y);
    pos_x = newpoint[0];
    pos_y = newpoint[1];
    

    /* catch case for edge of board */
    if (pos_x >= 0 && pos_x <= 7 && pos_y >= 0 && pos_y <= 7) {
        if (our_playernum == 0) {
            int pos1_x;
            int pos1_y;
            int* nextpoint;

            switch (toppiece(board, pos_x, pos_y)) {
                case 'b':
                    nextpoint = moveindirection(direction, pos_x, pos_y);
                    pos1_x = nextpoint[0];
                    pos1_y = nextpoint[1];

                    /* catch case for edge of board */
                    if (pos1_x >= 0 && pos1_x <= 7 && pos1_y >= 0 && pos1_y <= 7) {
                        
                        if (toppiece(board, pos1_x, pos1_y) == ' ') {
                            // if first capture -> write pos:pos
                            if (strcmp(tower_move, "") == 0) {
                                char pre_pos[POSLEN];
                                snprintf(pre_pos, POSLEN, "%s", translate_pos(x, y));
                                char post_pos[POSLEN];
                                snprintf(post_pos, POSLEN, "%s", translate_pos(pos1_x, pos1_y));
                                snprintf(tower_move, MAXMOVELEN, "%s:%s", pre_pos, post_pos);

                                flag_all_possible_moves[num_moves] = BETTERMOVE;
                            }
                            // otherwise we just want to write :pos onto the end
                            else {
                                // write ending pos into string
                                char temp_pos[POSLEN];
                                snprintf(temp_pos, POSLEN, "%s", translate_pos(pos1_x, pos1_y));

                                // write all previous partial moves into string
                                char already_moved[MAXMOVELEN];
                                snprintf(already_moved, MAXMOVELEN, "%s", tower_move);

                                // delete old partial moves
                                //memset(tower_move, 0, MAXMOVELEN);
                                
                                // concat previous partial moves with new capturing move
                                snprintf(tower_move, MAXMOVELEN, "%s:%s", already_moved, temp_pos);

                                // increment our move flag to show that this move is better
                                flag_all_possible_moves[num_moves]++;
                            }

                            // make temp board for recursion
                            char temp_board[BOARDSIZE][BOARDSIZE][MAXTOWERLEN];
                            memcpy(temp_board, board, sizeof(char)*MAXTOWERLEN*BOARDSIZE*BOARDSIZE);

                            // make first move on the temp board
                            // remove our whole tower at x, y
                            memset(temp_board[x][y], 0, MAXTOWERLEN);

                            // remove top piece from captured tower 
                            removetoppiece(temp_board, pos_x, pos_y);                        

                            // test if piece became queen
                            if (pos1_y == 7) {
                                // write our new tower at pos1_x, pos1_y
                                snprintf(temp_board[pos1_x][pos1_y], MAXTOWERLEN, "W");
                                // TODO: you're a queen now! do a queen function...
                            }
                            else {
                                // write our new tower at pos1_x, pos1_y
                                snprintf(temp_board[pos1_x][pos1_y], MAXTOWERLEN, "w");

                                // check for further capture moves, recursion!
                                for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                                    possibletowermoves(temp_board, pos1_x, pos1_y, direction);
                                }
                            }  
                        }
                    }
                    break;
                case 'B':
                    nextpoint = moveindirection(direction, pos_x, pos_y);
                    pos1_x = nextpoint[0];
                    pos1_y = nextpoint[1];

                    /* catch case for edge of board */
                    if (pos1_x >= 0 && pos1_x <= 7 && pos1_y >= 0 && pos1_y <= 7) {
                        
                        if (toppiece(board, pos1_x, pos1_y) == ' ') {
                            // if first capture -> write pos:pos
                            if (strcmp(tower_move, "") == 0) {
                                char pre_pos[POSLEN];
                                snprintf(pre_pos, POSLEN, "%s", translate_pos(x, y));
                                char post_pos[POSLEN];
                                snprintf(post_pos, POSLEN, "%s", translate_pos(pos1_x, pos1_y));
                                snprintf(tower_move, MAXMOVELEN, "%s:%s", pre_pos, post_pos);

                                flag_all_possible_moves[num_moves] = EVENBETTERMOVE;
                            }
                            // otherwise we just want to write :pos onto the end
                            else {
                                // write ending pos into string
                                char temp_pos[POSLEN];
                                snprintf(temp_pos, POSLEN, "%s", translate_pos(pos1_x, pos1_y));

                                // write all previous partial moves into string
                                char already_moved[MAXMOVELEN];
                                snprintf(already_moved, MAXMOVELEN, "%s", tower_move);

                                // delete old partial moves
                                //memset(tower_move, 0, MAXMOVELEN);
                                
                                // concat previous partial moves with new capturing move
                                snprintf(tower_move, MAXMOVELEN, "%s:%s", already_moved, temp_pos);

                                // increment our move flag to show that this move is better
                                flag_all_possible_moves[num_moves]++;
                                flag_all_possible_moves[num_moves]++;
                            }

                            // make temp board for recursion
                            char temp_board[BOARDSIZE][BOARDSIZE][MAXTOWERLEN];
                            memcpy(temp_board, board, sizeof(char)*MAXTOWERLEN*BOARDSIZE*BOARDSIZE);

                            // make first move on the temp board
                            // remove our whole tower at x, y
                            memset(temp_board[x][y], 0, MAXTOWERLEN);

                            // remove top piece from captured tower 
                            removetoppiece(temp_board, pos_x, pos_y);                        

                            // test if piece became queen
                            if (pos1_y == 7) {
                                // write our new tower at pos1_x, pos1_y
                                snprintf(temp_board[pos1_x][pos1_y], MAXTOWERLEN, "W");
                                // TODO: you're a queen now! do a queen function...
                            }
                            else {
                                // write our new tower at pos1_x, pos1_y
                                snprintf(temp_board[pos1_x][pos1_y], MAXTOWERLEN, "w");

                                // check for further capture moves, recursion!
                                for(int direction=TOPLEFT; direction<=BOTTOMRIGHT; direction++){
                                    possibletowermoves(temp_board, pos1_x, pos1_y, direction);
                                }
                            }  
                        }
                    }
                    break;
                case ' ':
                    if (direction == TOPLEFT || direction == TOPRIGHT) {
                        if (strcmp(tower_move, "") == 0) {
                            char pre_pos[POSLEN];
                            snprintf(pre_pos, POSLEN, "%s", translate_pos(x, y));
                            char post_pos[POSLEN];
                            snprintf(post_pos, POSLEN, "%s", translate_pos(pos_x, pos_y));
                            snprintf(tower_move, MAXMOVELEN, "%s:%s", pre_pos, post_pos);

                            flag_all_possible_moves[num_moves] = BADMOVE;
                        }
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
char toppiece(char board[BOARDSIZE][BOARDSIZE][MAXTOWERLEN], int x, int y){
    char piece = ' ';
    if (board[x][y][strlen(board[x][y])-1]) {
        piece = board[x][y][strlen(board[x][y])-1];
    }
    return piece;
}

/* removes the top piece from a tower at a given location */
void removetoppiece(char board[BOARDSIZE][BOARDSIZE][MAXTOWERLEN], int x, int y) {
    // int topindex = 0;
    // memmove(&board[x][y][topindex], &board[x][y][topindex+1], strlen(board[x][y]) - topindex);

    int lastindex = strlen(board[x][y]) - 1;
    char new_twr[MAXTOWERLEN];
    snprintf(new_twr, MAXTOWERLEN, "%s", board[x][y]);
    new_twr[lastindex] = '\0';
    memset(board[x][y], 0, MAXTOWERLEN);
    snprintf(board[x][y], MAXTOWERLEN, "%s", new_twr);
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
