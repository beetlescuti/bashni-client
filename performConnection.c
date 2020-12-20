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
// TODO: Whenever I run it with PLAYER = 2 it says, that there is no free player ... there seems to be an off by one error; if I run it with Player =1 I always get assigned Player 2

char server_msg[1024];
char client_msg[1024];
int result;
int packets;
int bytes_received;
char* dividedServerMsg[100];






void receiveServerMsg(int socket_file_descriptor) {
    /* reset old server_msg */
    result = 0;
    packets = 0;
    bytes_received = 0;
    memset(server_msg, 0, 1024);

    /* receive all packets and concat onto server_msg */
    do {
        result = recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
        packets++;
        if (result > 0 ) {
            bytes_received+=result;
        }
        else {
            printf("Error: %i \n", result);
            exit(EXIT_FAILURE);
        }
        if (bytes_received >= 1024) {
            printf("Buffer overflow");
        break;
        }
    } while(server_msg[bytes_received-1] != '\n');
}

int serverConnect(int socket_file_descriptor, char game_id[], int player) {

    /* Enter an infinite while loop that calls receiveServerMsg() and then filters the
       result into the correct switch/case */
    while (1) {
        /* Receive Bytestream from Socket to parse into cases*/
        receiveServerMsg(socket_file_descriptor);

        /*  */
        switch (server_msg[0]) {
            case '+':
                // Divide ServerMsg into tokens so they can be adressed separately
                *dividedServerMsg = divideServerMsg(server_msg);

                for (int i = 0; i < 100; i++) {
                  char *current = dividedServerMsg[i];
                  char stringMatch[12];

                  if (dividedServerMsg[i] != NULL) {

                    printf("S: %s\n", dividedServerMsg[i]);

                    if (sscanf(current, "+ MNM Gameserver %*s accepting %s", stringMatch) == 1 ) {
                      /* send: client version, major version must match!! */
                      strcpy(client_msg, "VERSION 2.3\n");
                      if (send(socket_file_descriptor, client_msg, strlen(client_msg), 0) < 0) {
                           printf("send failed\n");
                      }
                      else {
                           printf("C: %s", client_msg);
                      }
                      memset(stringMatch, 0, 12);
                      memset(client_msg, 0, 1024);
                    }

                    else if (sscanf(current, "+ Client version accepted - please send %s to join", stringMatch) == 1 ){
                      strcpy(client_msg, "ID ");
                      strcat(client_msg, game_id);
                      strcat(client_msg, "\n");

                      if (send(socket_file_descriptor, client_msg, strlen(client_msg), 0) < 0) {
                           printf("send failed\n");
                      }
                      else {
                           printf("C: %s", client_msg);
                      }

                    }

                    else {
                      perror("sscanf");
                      fprintf(stderr, "could not parse\n");
                      exit(EXIT_FAILURE);
                    }
                  }
                }
            case '-':
                //For debugging
                //printf("%c\n", server_msg[0]);
                printf("Error: %s\n", server_msg);
                break;
            default:
                break;
        }
    }
}

char* divideServerMsg(char *server_msg){
  char *saveptr;
  char *tokenArray[1024];

  int num_tokens = 0;
  char *token;
  char *current = server_msg;

  while( (token = strtok_r(current, "\n", &saveptr)) != NULL){
    tokenArray[num_tokens] = token;
    num_tokens++;
    current = NULL;
  }

  return *tokenArray;
};

// int serverConnect(int socket_file_descriptor, char game_id[], int player) {

//     char server_msg[1024];
//     int result = 0;
//     int packets = 0;
//     int bytes_received= 0;
//     memset(server_msg, 0, 1024);

//     /* receive: is server accepting connections? */
//     do {
//         result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
//         packets++;
//         if (result > 0 ) {
//             bytes_received+=result;
//         }
//         else {
//             printf("Error: %i \n", result);
//         }
//         if (bytes_received >= 1024) {
//             printf("Buffer overflow");
//         break;
//         }
//     } while(server_msg[bytes_received-1] != '\n');

//     // for debugging only
//     // printf(  "S: %s bytes: %i packets: %i \n" , server_msg, bytes_received, packets);
//     printf("S: %s" , server_msg);

//     /* send: client version, major version must match!! */
//     char client_version[14] = "VERSION 2.3\n";
//     if (send(socket_file_descriptor, client_version, strlen(client_version), 0) < 0) {
//         printf("send failed\n");
//     }
//     else {
//         printf("C: %s", client_version);
//     }

//     /* empty previous server message */
//     result = 0;
//     packets = 0;
//     bytes_received= 0;
//     memset(server_msg, 0, 1024);

//     /* receive: is client version accepted? */
//     do {
//         result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
//         packets++;
//         if (result > 0 ) {
//             bytes_received+=result;
//         }
//         else {
//             printf("Error: %i \n", result);
//         }
//         if (bytes_received >= 1024) {
//             printf("Buffer overflow");
//             break;
//         }
//     } while(server_msg[bytes_received-1] != '\n');

//     // for debugging only
//     // printf(  "S: %s bytes: %i packets: %i \n" , server_msg, bytes_received, packets);
//     printf("S: %s" , server_msg);


//     /* empty previous server message */
//     result = 0;
//     packets = 0;
//     bytes_received= 0;
//     memset(server_msg, 0, 1024);

//     //preparing game_id_msg
//     char game_id_msg[18];
//     strcpy(game_id_msg, "ID ");
//     strcat(game_id_msg, game_id);
//     strcat(game_id_msg, "\n");


//     // sending game_id
//     if (send(socket_file_descriptor, game_id_msg, strlen(game_id_msg), 0) < 0) {
//         printf("send failed\n");
//     }
//     else {
//         printf("C: %s", game_id_msg);
//     }

//     //receive: is game-id accepted?
//     do {
//         result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
//         packets++;
//         if (result > 0 ) {
//             bytes_received+=result;
//         }
//         else {
//             printf("Error: %i \n", result);
//         }
//         if (bytes_received >= 1024) {
//             printf("Buffer overflow");
//             break;
//         }
//     } while(server_msg[bytes_received-1] != '\n');

//     //print server_msg by separating new statements
//     printf("S: ");
//     for (size_t i = 0; i < strlen(server_msg); i++) {
//       if (server_msg[i] == '+' && i != 0) {
//         printf("S: %c", server_msg[i]);
//       }
//       else {
//         printf("%c", server_msg[i]);
//       }
//     }

//     //preparing player_msg
//     char player_msg[10] = "PLAYER";
//     if (player > 0){
//       char player_string[2];
//       snprintf(player_string, 2, "%d", player - 1);
//       strcat(player_msg, " ");
//       strcat(player_msg, player_string);
//     }
//     strcat(player_msg, "\n");

//     printf("%s", player_msg);

//     //sending player_msg

//     if (send(socket_file_descriptor, player_msg, strlen(player_msg), 0) < 0) {
//         printf("send failed\n");
//     }
//     else {
//         printf("C: %s", player_msg);
//     }

//     /* empty previous server message */
//     result = 0;
//     packets = 0;
//     bytes_received= 0;
//     memset(server_msg, 0, 1024);

//     //receive: is player accepted?
//     do {
//         result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
//         packets++;
//         if (result > 0 ) {
//             bytes_received+=result;
//         }
//         else {
//             printf("Error: %i \n", result);
//         }
//         if (bytes_received >= 1024) {
//             printf("Buffer overflow");
//             break;
//         }
//     } while(server_msg[bytes_received-1] != '\n');

//     //print server_msg by separating new statements
//     printf("S: ");
//     for (size_t i = 0; i < strlen(server_msg); i++) {
//       if (server_msg[i] == '+' && i != 0) {
//         printf("S: %c", server_msg[i]);
//       }
//       else {
//         printf("%c", server_msg[i]);
//       }
//     }

//     return 0;
// }
