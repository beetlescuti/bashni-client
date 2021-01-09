#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "performConnection.h"
#include "sharedMemory.h"

// TODO: implement wait sequence

#define MSGLEN 1024
#define MATCHLEN 1024
#define TOKENLEN 128
#define MAXPLAYERS 32

char server_msg[MSGLEN];
char client_msg[MSGLEN];
int result;
int packets;
int bytes_received;
char** dividedServerMsg;
char* tokenArray[TOKENLEN];

// The variables below are used to store sscanf matches
char server_version[MATCHLEN];
char server_placeholder[MATCHLEN];
char server_gamename[MATCHLEN];
int our_playernum;
char our_playername[MATCHLEN];
int server_totalplayers;

// An array of player structs
player all_players[MAXPLAYERS];



char stringMatch[MATCHLEN];
int intMatch = -1;
char position[3];
char piece;

game_info our_info;

game_info serverConnect(int socket_file_descriptor, char game_id[], int player) {

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

                // Look at each of the tokens separately
                for (int i = 0; i < TOKENLEN; i++) {
                    char *current = dividedServerMsg[i];

                    if (dividedServerMsg[i] != NULL) {
                        printf("S: %s\n", current);

                        // All the possible Server Messages and their corresponding actions by the client follow
                    
                        if (sscanf(current, "+ MNM Gameserver %s accepting connections", server_version) == 1) {
                            // send: client version, major version must match!!
                            snprintf(client_msg, MSGLEN, "VERSION 2.3\n");
                            sendClientMsg(socket_file_descriptor);
                        }

                        else if (sscanf(current, "+ Client version accepted - please send %s to join", server_placeholder) == 1){
                            snprintf(client_msg, MSGLEN, "ID %s\n", game_id);
                            sendClientMsg(socket_file_descriptor);

                            // clear for reuse
                            memset(server_placeholder, 0, MATCHLEN);
                        }
                        
                        // Check if the game that we want to join is a Bashni game. If it is, send preferred player number
                        else if (sscanf(current, "+ PLAYING %s", server_gamename) == 1){
                            if (strcmp(server_gamename, "Bashni") == 0) {
                                // Since game name could be anything, there's nothing to parse
                                // so I'm just choosing to print the next server token and incrementing i to skip it in the next for-loop cyle
                                printf("S: %s\n", dividedServerMsg[i+1]);

                                // put the game name into our data structure
                                // the "+= 2" is to increment the pointer so our string doesn't include the "+ " prefix
                                snprintf(our_info.game_name, MATCHLEN, "%s", dividedServerMsg[i+1] += 2);

                                i++;

                                //if a player has been specified in the config then send it, otherwise simply send player
                                if (player > 0) {
                                    snprintf(client_msg, MSGLEN, "PLAYER %d\n", player-1);
                                }
                                else {
                                    snprintf(client_msg, MSGLEN, "PLAYER\n");
                                }
                                // sending
                                sendClientMsg(socket_file_descriptor);
                            }
                            else {
                                perror("sscanf");
                                fprintf(stderr, "Wrong gametype, connect to a Bashni game\n");
                                exit(EXIT_FAILURE);
                            }
                        }

                        else if (sscanf(current, "+ YOU %d %s", &our_playernum, our_playername) == 2) {
                            // put the player number and name into our data structure
                            our_info.our_playernum = our_playernum;
                            // put our player info into the array of players
                            all_players[0].playernum = our_playernum;
                            snprintf(all_players[0].name, MATCHLEN, "%s", our_playername);
                            all_players[0].flag = 1;

                            memset(stringMatch, 0, MATCHLEN);
                        }

                        else if (sscanf(current, "+ TOTAL %d", &server_totalplayers) == 1){
                            // put the total number of players into our data structure
                            our_info.total_players = server_totalplayers;

                            // iterate through all additional players
                            // if total players is 1 then this code is skipped and we procede
                            for (size_t j = 1; j < server_totalplayers; j++) {
                                // scan every other player line and store in array of players

                                sscanf(dividedServerMsg[i+j], "+ %d %[^\t\n] %d", &all_players[j].playernum, all_players[j].name, &all_players[j].flag);
                                
                                // time to remove the trailing flag on the name
                                char namewithoutbool[NAMELEN];
                                snprintf(namewithoutbool, NAMELEN, "%s", all_players[j].name);
                                namewithoutbool[strlen(namewithoutbool) - 2] = '\0';
                            }

                            i = i + server_totalplayers - 1;

                        }

                        else if (sscanf(current, "+ ENDPLA%s", server_placeholder) == 1){
                            memset(server_placeholder, 0, MATCHLEN);
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
                            snprintf(client_msg, MSGLEN, "THINKING\n");
                            // sending
                            sendClientMsg(socket_file_descriptor);
                        }

                        else if (sscanf(current, "+ OKTHIN%s", stringMatch) == 1){
                            memset(stringMatch, 0, MATCHLEN);

                            /* ---------------------- arbitrary end-point for shmem testing ---------------------- */
                            // TODO: make these non-arbitrary
                            // snprintf(our_info.game_name, NAMELEN, "test_game");
                            our_info.total_players = 2;
                            our_info.our_playernum = 0;
                            our_info.connector_id = 55190;
                            our_info.thinker_id = 55187;

                            return our_info;

                            exit(EXIT_SUCCESS);

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
                // For debugging
                // printf("%c\n", server_msg[0]);
                printf("Error: %s\n", server_msg);
                break;
            default:
                break;
        }
    }
}

// receives Server Messages and ensures message is complete
void receiveServerMsg(int socket_file_descriptor) {
    /* reset old server_msg */
    result = 0;
    packets = 0;
    bytes_received = 0;
    memset(server_msg, 0, MSGLEN);

    /* receive all packets and concat onto server_msg */
    do {
        result = recv(socket_file_descriptor, &server_msg[bytes_received], MSGLEN-bytes_received, 0);
        packets++;
        if (result > 0 ) {
            bytes_received+=result;
        }
        else {
            printf("Error: %i \n", result);
            exit(EXIT_FAILURE);
        }
        if (bytes_received >= MSGLEN) {
            printf("Buffer overflow");
        break;
        }
    } while(server_msg[bytes_received-1] != '\n');
}

// divides Server Message into an array of strings, so they can be parsed separately
char** divideServerMsg(char *server_msg, char **tokenArray){
    memset(tokenArray, 0, MSGLEN);
    char *saveptr;

    int num_tokens = 0;
    char *token;
    char *current = server_msg;

    while((token = strtok_r(current, "\n", &saveptr)) != NULL){
        tokenArray[num_tokens] = token;
        num_tokens++;
        current = NULL;
    }

    return tokenArray;
};

// sends a client message to the server
void sendClientMsg(int socket_file_descriptor) {
    if (send(socket_file_descriptor, client_msg, strlen(client_msg), 0) < 0) {
        printf("Send Client message failed\n");
    }
    else {
        printf("C: %s", client_msg);
    }

    // clear the variables for use in the next send
    memset(stringMatch, 0, MATCHLEN);
    memset(client_msg, 0, MSGLEN);
}