#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "performConnection.h"

// TODO: wrap send client_msg into separate function for readability

char server_msg[1024];
char client_msg[1024];
int result;
int packets;
int bytes_received;
char** dividedServerMsg;
char* tokenArray[100];

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

        /* Check first byte of server_msg to see if Server is responding with a "+ ....." or an Error message */
        switch (server_msg[0]) {
            case '+':
                // Divide ServerMsg into tokens so they can be parsed separately
                dividedServerMsg = divideServerMsg(server_msg, tokenArray);

                //The variables below are used to store sscanf matches
                char stringMatch[100];
                int intMatch = -1;
                char position[3];
                char piece;

                //Look at each of the tokens separately
                for (int i = 0; i < 100; i++) {
                  char *current = dividedServerMsg[i];

                  if (dividedServerMsg[i] != NULL) {
                    printf("S: %s\n", dividedServerMsg[i]);

                    //All the possible Server Messages and their corresponding actions by the client follow

                    if (sscanf(current, "+ MNM Gameserver %*s accepting %s", stringMatch) == 1 ) {

                      /* send: client version, major version must match!! */
                      strcpy(client_msg, "VERSION 2.3\n");
                      if (send(socket_file_descriptor, client_msg, strlen(client_msg), 0) < 0) {
                           printf("send failed\n");
                      }
                      else {
                           printf("C: %s", client_msg);
                      }
                      memset(stringMatch, 0, 100);
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
                      memset(stringMatch, 0, 100);
                      memset(client_msg, 0, 1024);
                    }

                    //Check if the game that we want to join is a Bashni game. If it is, send preferred player number
                    else if (sscanf(current, "+ PLAYING %s", stringMatch) == 1){
                      if (strcmp(stringMatch, "Bashni") == 0) {
                        //Since game name could be anything, there's nothing to parse
                        //so I'm just choosing to print the next server token and incrementing i to skip it in the next for-loop cyle
                        printf("S: %s\n", dividedServerMsg[i+1]);
                        i++;
                        //preparing client_msg
                        strcpy(client_msg, "PLAYER");
                        if (player > 0){
                          char player_string[2];
                          snprintf(player_string, 2, "%d", player - 1);
                          strcat(client_msg, " ");
                          strcat(client_msg, player_string);
                        }
                        strcat(client_msg, "\n");

                        //sending
                        if (send(socket_file_descriptor, client_msg, strlen(client_msg), 0) < 0) {
                            printf("send failed\n");
                        }
                        else {
                            printf("C: %s", client_msg);
                        }
                      }
                      else {
                        perror("sscanf");
                        fprintf(stderr, "Wrong gametype, connect to a Bashni game\n");
                        exit(EXIT_FAILURE);
                      }
                      memset(stringMatch, 0, 100);
                      memset(client_msg, 0, 1024);
                    }

                    else if (sscanf(current, "+ YOU %d %s", &player, stringMatch) == 2) {
                      memset(stringMatch, 0, 100);
                    }

                    else if (sscanf(current, "+ TOTAL %d", &intMatch) == 1){
                      //We know how many players there are but can't really parse player number or player name,
                      // so just print them and skip to after the players are listed
                      for (size_t j = 1; j < intMatch; j++) {

                        printf("S: %s\n", dividedServerMsg[i+j]);
                      }
                      i = i + intMatch - 1;
                      intMatch = -1;
                    }

                    else if (sscanf(current, "+ ENDPLA%s", stringMatch) == 1){
                      memset(stringMatch, 0, 100);
                    }

                    else if (sscanf(current, "+ MOVE %d", &intMatch) == 1){
                      intMatch = -1;
                    }

                    else if (sscanf(current, "+ PIECESLIST %d", &intMatch) == 1){
                      // TODO: Maybe parse all the pieces in here, instead of in the next else-if statement,
                      // since we know how many pieces are following?
                      intMatch = -1;
                    }

                    else if (sscanf(current, "+ %c@%s", &piece, position) == 2){
                      memset(position, 0, 3);
                      piece = '0';
                    }

                    else if (sscanf(current, "+ ENDPIECES%s", stringMatch) == 1){
                      strcpy(client_msg, "THINKING\n");
                      //sending
                      if (send(socket_file_descriptor, client_msg, strlen(client_msg), 0) < 0) {
                          printf("send failed\n");
                      }
                      else {
                          printf("C: %s", client_msg);
                      }
                      memset(stringMatch, 0, 100);
                      memset(client_msg, 0, 1024);
                    }

                    else if (sscanf(current, "+ OKTHIN%s", stringMatch) == 1){
                      memset(stringMatch, 0, 100);
                    }

                    else {
                      perror("sscanf");
                      fprintf(stderr, "could not parse\n");
                      exit(EXIT_FAILURE);
                    }
                  }
                }
                break;
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

// divides Server Message into an array of strings, so they can be parsed separately
char** divideServerMsg(char *server_msg, char **tokenArray){
  memset(tokenArray, 0, 1024);
  char *saveptr;

  int num_tokens = 0;
  char *token;
  char *current = server_msg;

  while( (token = strtok_r(current, "\n", &saveptr)) != NULL){
    tokenArray[num_tokens] = token;
    num_tokens++;
    current = NULL;
  }

  return tokenArray;
};
