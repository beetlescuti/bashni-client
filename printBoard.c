#include <stdio.h>
#include <string.h>

#include "sharedMemory.h"
#include "thinker.h"

/* takes an all_info struct and prints the board in a readable manner */
void printboard(char board[8][8][25]) {
    printf("\n   A B C D E F G H\n");
    printf(" +-----------------+\n");
    printf("8| . %c . %c . %c . %c |8\n", toppiece(board, 1, 7), toppiece(board, 3, 7), toppiece(board, 5, 7),
           toppiece(board, 7, 7));
    printf("7| %c . %c . %c . %c . |7\n", toppiece(board, 0, 6), toppiece(board, 2, 6), toppiece(board, 4, 6),
           toppiece(board, 6, 6));
    printf("6| . %c . %c . %c . %c |6\n", toppiece(board, 1, 5), toppiece(board, 3, 5), toppiece(board, 5, 5),
           toppiece(board, 7, 5));
    printf("5| %c . %c . %c . %c . |5\n", toppiece(board, 0, 4), toppiece(board, 2, 4), toppiece(board, 4, 4),
           toppiece(board, 6, 4));
    printf("4| . %c . %c . %c . %c |4\n", toppiece(board, 1, 3), toppiece(board, 3, 3), toppiece(board, 5, 3),
           toppiece(board, 7, 3));
    printf("3| %c . %c . %c . %c . |3\n", toppiece(board, 0, 2), toppiece(board, 2, 2), toppiece(board, 4, 2),
           toppiece(board, 6, 2));
    printf("2| . %c . %c . %c . %c |2\n", toppiece(board, 1, 1), toppiece(board, 3, 1), toppiece(board, 5, 1),
           toppiece(board, 7, 1));
    printf("1| %c . %c . %c . %c . |1\n", toppiece(board, 0, 0), toppiece(board, 2, 0), toppiece(board, 4, 0),
           toppiece(board, 6, 0));
    printf(" +-----------------+\n");
    printf("   A B C D E F G H\n");

    printf("\nwhite towers\n");
    printf("------------\n");
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (toppiece(board, x, y) == 'w' || toppiece(board, x, y) == 'W') {
                printf("%s: %s\n", translate_pos(x, y), board[x][y]);
            }
        }
    }

    printf("\nblack towers\n");
    printf("------------\n");
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (toppiece(board, x, y) == 'b' || toppiece(board, x, y) == 'B') {
                printf("%s: %s\n", translate_pos(x, y), board[x][y]);
            }
        }
    }
    printf("\n");
}
