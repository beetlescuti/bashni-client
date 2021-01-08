#include "sharedMemory.h"

#ifndef performConnection
#define performConnection

game_info serverConnect(int socket_file_descriptor, char game_id[], int player);
void receiveServerMsg(int socket_file_descriptor);
char** divideServerMsg(char *server_msg, char **tokenArray);
void sendClientMsg(int socket_file_descriptor);

#endif
