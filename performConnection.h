#include "sharedMemory.h"

#ifndef performConnection
#define performConnection

#define MSGLEN 1024

int serverConnect(int socket_file_descriptor, char game_id[], int player, int * shmid_ptr);
void receiveServerMsg(int socket_file_descriptor);
char** divideServerMsg(char *server_msg, char **tokenArray);
void sendClientMsg(int socket_file_descriptor);
void writetower(char board[BOARDSIZE][BOARDSIZE][MAXTOWERLEN], int x, int y, char *new_piece);

#endif
