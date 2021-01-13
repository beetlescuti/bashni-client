//
// Created by svea on 13.01.21.
//

#include "sharedMemory.h"

 void printboard(all_info current_status){
    printf("   A B C D E F G H");
    printf(" +-----------------+");
    printf("8| . %d . %d . %d . %d |8", current_status.board[1][7], current_status.board[3][7],current_status.board[5][7], current_status.board[7][7])
     printf("6| . %d . %d . %d . %d |6", current_status.board[1][7], current_status.board[3][7],current_status.board[5][7], current_status.board[7][7])
     printf("4| . %d . %d . %d . %d |4", current_status.board[1][7])
     printf("2| . %d . %d . %d . %d |2", current_status.board[1][7])

}

