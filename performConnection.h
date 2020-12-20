#ifndef performConnection
#define performConnection

int serverConnect(int socket_file_descriptor, char game_id[], int player);
void receiveServerMsg(int socket_file_descriptor);
char* divideServerMsg(char *server_msg);

#endif
