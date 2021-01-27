#include <stdio.h>

#include "sharedMemory.h"
#include <string.h>
/* takes an all_info struct and prints the board in a readable manner */
void printboard(char board[8][8][25]){
    printf("\n\n   A B C D E F G H\n");
    printf(" +-----------------+\n");
    printf("8| . %c . %c . %c . %c |8\n",board[1][7][strlen(board[1][7])-1], board[3][7][strlen(board[3][7])-1], board[5][7][strlen(board[5][7])-1], board[7][7][strlen(board[7][7])-1]);
    printf("7| %c . %c . %c . %c . |7\n",board[0][6][strlen(board[0][6])-1], board[2][6][strlen(board[2][6])-1], board[4][6][strlen(board[4][6])-1], board[6][6][strlen(board[6][6])-1]);
    printf("6| . %c . %c . %c . %c |6\n",board[1][5][strlen(board[1][5])-1], board[3][5][strlen(board[3][5])-1], board[5][7][strlen(board[5][5])-1], board[7][5][strlen(board[7][5])-1]);
    printf("5| %c . %c . %c . %c . |5\n",board[0][4][strlen(board[0][4])-1], board[2][4][strlen(board[2][4])-1], board[4][4][strlen(board[4][4])-1], board[6][4][strlen(board[6][4])-1]);
    printf("4| . %c . %c . %c . %c |4\n",board[1][3][strlen(board[1][3])-1], board[3][3][strlen(board[3][3])-1], board[5][7][strlen(board[5][3])-1], board[7][3][strlen(board[7][3])-1]);
    printf("3| %c . %c . %c . %c . |3\n",board[0][2][strlen(board[0][2])-1], board[2][2][strlen(board[2][2])-1], board[4][2][strlen(board[4][2])-1], board[6][2][strlen(board[6][2])-1]);
    printf("2| . %c . %c . %c . %c |2\n",board[1][1][strlen(board[1][1])-1], board[3][1][strlen(board[3][1])-1], board[5][1][strlen(board[5][1])-1], board[7][1][strlen(board[7][1])-1]);
    printf("1| %c . %c . %c . %c . |1\n",board[0][0][strlen(board[0][0])-1], board[2][0][strlen(board[2][0])-1], board[4][0][strlen(board[4][0])-1], board[6][0][strlen(board[6][0])-1]);;
    printf(" +-----------------+\n");
    printf("   A B C D E F G H\n");
}

