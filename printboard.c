#include <stdio.h>

#include "sharedMemory.h"

// TODO: replace the -1's with b or B respectively

/* takes an all_info struct and prints the board in a readable manner */
void printboard(int board[8][8]){
    printf("\n\n   A B C D E F G H\n");
    printf(" +-----------------+\n");
    printf("8| . %d . %d . %d . %d |8\n", board[1][7], board[3][7], board[5][7], board[7][7]);
    printf("7| %d . %d . %d . %d . |7\n", board[0][6], board[2][6], board[4][6], board[6][6]);
    printf("6| . %d . %d . %d . %d |6\n", board[1][5], board[3][5], board[5][5], board[7][5]);
    printf("5| %d . %d . %d . %d . |5\n", board[0][4], board[2][4], board[4][4], board[6][4]);
    printf("4| . %d . %d . %d . %d |4\n", board[1][3], board[3][3], board[5][3], board[7][3]);
    printf("3| %d . %d . %d . %d . |3\n", board[0][2], board[2][2], board[4][2], board[6][2]);
    printf("2| . %d . %d . %d . %d |2\n", board[1][1], board[3][1], board[5][1], board[7][1]);
    printf("1| %d . %d . %d . %d . |1\n", board[0][0], board[2][0], board[4][0], board[6][0]);
    printf(" +-----------------+\n");
    printf("   A B C D E F G H\n");
}

