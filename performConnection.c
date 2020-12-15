#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "performConnection.h"

// TODO: wrap receive message (with newline as end) into function
// TODO: continue with implementation of protocol
int serverConnect(int socket_file_descriptor, char game_id[], int player) {

    char server_msg[1024];
    int result = 0;
    int packets = 0;
    int bytes_received= 0;
    memset(server_msg, 0, 1024);

    /* receive: is server accepting connections? */
    do {
        result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
        packets++;
        if (result > 0 ) {
            bytes_received+=result;
        }
        else {
            printf("Error: %i \n", result);
        }
        if (bytes_received >= 1024) {
            printf("Buffer overflow");
        break;
        }
    } while(server_msg[bytes_received-1] != '\n');

    // for debugging only
    // printf(  "S: %s bytes: %i packets: %i \n" , server_msg, bytes_received, packets);
    printf("S: %s" , server_msg);

    /* send: client version, major version must match!! */
    char client_version[14] = "VERSION 2.3\n";
    if (send(socket_file_descriptor, client_version, strlen(client_version), 0) < 0) {
        printf("send failed\n");
    }
    else {
        printf("C: %s", client_version);
    }

    /* empty previous server message */
    result = 0;
    packets = 0;
    bytes_received= 0;
    memset(server_msg, 0, 1024);

    /* receive: is client version accepted? */
    do {
        result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
        packets++;
        if (result > 0 ) {
            bytes_received+=result;
        }
        else {
            printf("Error: %i \n", result);
        }
        if (bytes_received >= 1024) {
            printf("Buffer overflow");
            break;
        }
    } while(server_msg[bytes_received-1] != '\n');

    // for debugging only
    // printf(  "S: %s bytes: %i packets: %i \n" , server_msg, bytes_received, packets);
    printf("S: %s" , server_msg);


    /* empty previous server message */
    result = 0;
    packets = 0;
    bytes_received= 0;
    memset(server_msg, 0, 1024);

    //preparing game_id_msg
    char game_id_msg[18];
    strcpy(game_id_msg, "ID ");
    strcat(game_id_msg, game_id);
    strcat(game_id_msg, "\n");

    //sending game_id
    if (send(socket_file_descriptor, game_id_msg, strlen(game_id_msg), 0) < 0) {
        printf("send failed\n");
    }
    else {
        printf("C: %s", game_id_msg);
    }

    //receive: is game-id accepted?
    do {
        result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
        packets++;
        if (result > 0 ) {
            bytes_received+=result;
        }
        else {
            printf("Error: %i \n", result);
        }
        if (bytes_received >= 1024) {
            printf("Buffer overflow");
            break;
        }
    } while(server_msg[bytes_received-1] != '\n');

    //print server_msg by separating new statements
    printf("S: ");
    for (size_t i = 0; i < strlen(server_msg); i++) {
      if (server_msg[i] == '+' && i != 0) {
        printf("S: %c", server_msg[i]);
      }
      else {
        printf("%c", server_msg[i]);
      }
    }

    //preparing player_msg
    char player_msg[10] = "PLAYER";
    if (player > 0){
      char player_string[3];
      snprintf(player_string, 3, "%d", player);
      strcat(player_msg, " ");
      strcat(player_msg, player_string);
    }
    strcat(player_msg, "\n");


    //sending player_msg

    if (send(socket_file_descriptor, player_msg, strlen(player_msg), 0) < 0) {
        printf("send failed\n");
    }
    else {
        printf("C: %s", player_msg);
    }

    /* empty previous server message */
    result = 0;
    packets = 0;
    bytes_received= 0;
    memset(server_msg, 0, 1024);

    //receive: is player accepted?
    do {
        result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
        packets++;
        if (result > 0 ) {
            bytes_received+=result;
        }
        else {
            printf("Error: %i \n", result);
        }
        if (bytes_received >= 1024) {
            printf("Buffer overflow");
            break;
        }
    } while(server_msg[bytes_received-1] != '\n');

    //print server_msg by separating new statements
    printf("S: ");
    for (size_t i = 0; i < strlen(server_msg); i++) {
      if (server_msg[i] == '+' && i != 0) {
        printf("S: %c", server_msg[i]);
      }
      else {
        printf("%c", server_msg[i]);
      }
    }

    return 0;
}
