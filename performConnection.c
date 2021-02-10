#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <signal.h>
#include <limits.h>

#include "sysprakclient.h"
#include "performConnection.h"
#include "sharedMemory.h"
#include "printBoard.h"

// TODO: implement wait sequence
// TODO: What to do if no free player

#define MATCHLEN 1024
#define TOKENLEN 128
#define PIECESLEN 24

char server_msg[MSGLEN];
char client_msg[MSGLEN];
int result;
int packets;
int bytes_received;
char** dividedServerMsg;
char* tokenArray[TOKENLEN];
int boardsreceived = 0;

// The variables below are used to store sscanf matches
char server_version[MATCHLEN];
char server_placeholder[MATCHLEN];
char server_gamename[MATCHLEN];
int our_playernum;
char our_playername[MATCHLEN];
int server_totalplayers;
int server_max_moves;
int server_total_pieces;
char horizontal;
int vertical;
char piece[PIECESLEN];

// Create ptr to all_info struct for shared memory data
all_info * shm_info;

// Create necessary structs
all_info game_and_players;

int serverConnect(int socket_file_descriptor, char game_id[], int player, int * shmid_ptr) {

    /* Enter the PID and PPID into the game_info struct */
    game_and_players.game_info.connector_id = getpid();
    game_and_players.game_info.thinker_id = getppid();

    /* set file descriptor for thinker pipe */
    // Wait for message from server or from thinker
    fd_set readfds;

    // clear the socket set
    FD_ZERO(&readfds);

    // add filedescriptor as master socket to set
    FD_SET(socket_file_descriptor, &readfds);
    int max_sd = socket_file_descriptor;

    // add thinker filedescriptor as child to set
    FD_SET(fd[0], &readfds);

    // check for highest descriptor number
    if (fd[0] > max_sd){
        max_sd = fd[0];
    }

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
                                snprintf(game_and_players.game_info.game_name, MATCHLEN, "%s", dividedServerMsg[i+1] += 2);

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

                        else if (sscanf(current, "+ YOU %d %[^\t\n]", &our_playernum, our_playername) == 2) {
                            // put the player number and name into our data structure
                            game_and_players.game_info.our_playernum = our_playernum;
                            // put our player info into the array of players
                            game_and_players.all_players_info[0].playernum = our_playernum;
                            snprintf(game_and_players.all_players_info[0].name, MATCHLEN, "%s", our_playername);
                            printf("player name from server: %s \n",our_playername);
                            game_and_players.all_players_info[0].flag = 1;
                        }

                        /* 1. write total number of players to struct
                           2. loop through all players and write to players struct array
                           3. create shared memory segment with total players for size */
                        else if (sscanf(current, "+ TOTAL %d", &server_totalplayers) == 1){
                            // [1] put the total number of players into our data structure
                            game_and_players.game_info.total_players = server_totalplayers;

                            // [2] iterate through all additional players
                            //     if total players is 1 then this code is skipped and we procede
                            for (size_t j = 1; j < server_totalplayers; j++) {
                                // scan every player line and store in array of players
                                char namewithoutbool[NAMELEN];

                                sscanf(dividedServerMsg[i+j], "+ %d %[^\t\n] %d", &game_and_players.all_players_info[j].playernum, namewithoutbool, &game_and_players.all_players_info[j].flag);
                                printf("S: %s\n", dividedServerMsg[i+j]);
                                // time to remove the trailing flag on the name
                                namewithoutbool[strlen(namewithoutbool) - 2] = '\0';
                                snprintf(game_and_players.all_players_info[j].name, NAMELEN, "%s", namewithoutbool);
                            }
                            i = i + server_totalplayers - 1;

                            // [3] create shared memory and attach
                            // create shared memory segment
                            int shmid_for_info = create_shared_memory(game_and_players.game_info.total_players);

                            // give that new shmid to our other process
                            * shmid_ptr = shmid_for_info;

                            // attach to shared memory segment
                            shm_info = (all_info*) shmat(shmid_for_info, NULL, 0);
                            if (shm_info == (void *) -1) {
                                printf("Error attaching shared memory.\n");
                                perror("shmat");
                            }

                            printf("----------- SHMID [2] -----------\n");
                            printf("            %d\n", shmid_for_info);
                            printf("---------------------------------\n");
                        }

                        else if (sscanf(current, "+ ENDPLA%s", server_placeholder) == 1){
                            memset(server_placeholder, 0, MATCHLEN);
                        }

                        else if (sscanf(current, "+ MOVE %d", &server_max_moves) == 1){
                            game_and_players.game_info.max_moves = server_max_moves;
                        }

                        else if (sscanf(current, "+ PIECESLIST %d", &server_total_pieces) == 1){
                            game_and_players.game_info.total_pieces = server_total_pieces;
                            /* fill game board with 0's */
                            if (boardsreceived != 0) {
                                for (int x = 0; x < 8; x++) {
                                    for (int y = 0; y < 8; y++){
                                        memset(game_and_players.game_info.board[x][y], 0, MAXTOWERLEN);
                                    }
                                }
                            }
                        }

                        else if (sscanf(current, "+ %[^@]@%c%d", piece, &horizontal, &vertical) == 3){
                            switch (horizontal) {
                                case 'A':
                                    snprintf(game_and_players.game_info.board[0][vertical-1], MAXTOWERLEN, "%s" , piece);
                                    break;
                                case 'B':
                                    snprintf(game_and_players.game_info.board[1][vertical-1], MAXTOWERLEN, "%s" , piece);
                                    break;
                                case 'C':
                                    snprintf(game_and_players.game_info.board[2][vertical-1], MAXTOWERLEN, "%s" , piece);
                                    break;
                                case 'D':
                                    snprintf(game_and_players.game_info.board[3][vertical-1], MAXTOWERLEN, "%s" , piece);
                                    break;
                                case 'E':
                                    snprintf(game_and_players.game_info.board[4][vertical-1], MAXTOWERLEN, "%s" , piece);
                                    break;
                                case 'F':
                                    snprintf(game_and_players.game_info.board[5][vertical-1], MAXTOWERLEN, "%s" , piece);
                                    break;
                                case 'G':
                                    snprintf(game_and_players.game_info.board[6][vertical-1], MAXTOWERLEN, "%s" , piece);
                                    break;
                                case 'H':
                                    snprintf(game_and_players.game_info.board[7][vertical-1], MAXTOWERLEN, "%s" , piece);
                                    break;
                                default:
                                    printf("Not a valid position");
                                    break;
                            }

                            // clear those variables
                            memset(piece, 0, PIECESLEN);
                            horizontal = -1;
                            vertical = -1;
                        }

                        else if (sscanf(current, "+ ENDPIECES%s", server_placeholder) == 1){
                            snprintf(client_msg, MSGLEN, "THINKING\n");
                            // sending
                            sendClientMsg(socket_file_descriptor);
                            memset(server_placeholder, 0, MATCHLEN);
                        }

                        else if (sscanf(current, "+ OKTHI%s", server_placeholder) == 1){
                            memset(server_placeholder, 0, MATCHLEN);

                            // write new gameboard into shared memory
                            shm_info->game_info = game_and_players.game_info; 

                            if (boardsreceived == 0) {
                                // add our local structs to the shared memory segment
                                for (size_t n = 0; n <= game_and_players.game_info.total_players - 1; n++) {
                                    shm_info->all_players_info[n] = game_and_players.all_players_info[n];
                                }
                            }
                            // TEMPORARY!!!
                            // increment boardsreceived after every move so game can be manually stopped after a num of moves
                            boardsreceived++;

                            // set think-flag in shared memory
                            shm_info->game_info.think_flag = 1;

                            // send signal to parent that game info is ready
                            kill(game_and_players.game_info.thinker_id, SIGUSR1);

                            // read next move
                            char rcv_move[MSGLEN];                            

                            // wait for activity in one of the two sockets
                            int activity;
                            activity = select(max_sd+1, &readfds, NULL, NULL, NULL);

                            if (activity == -1)
                                perror("select()");
                            else if (activity) {
                                if (FD_ISSET(fd[0] , &readfds)) {
                                    printf("Data is available now.\n");
                                    read(fd[0],  rcv_move, MSGLEN);
                                    printf("received move: %s", rcv_move);
                                    snprintf(client_msg, MSGLEN, "PLAY %s", rcv_move);
                                    sendClientMsg(socket_file_descriptor);
                                    }
                                else if(FD_ISSET(socket_file_descriptor , &readfds)){
                                printf("Data from server.\n");
                                receiveServerMsg(socket_file_descriptor);
                                printf("Error: %s\n", server_msg);
                                }
                            }


                            // printf("%d \n", activity);

                            // TEMPORARY!!
                            // This should eventually be moved to the GAME OVER Protokoll
                            if (boardsreceived == 3) {
                                // detach from shared memory
                                shmdt(shmid_ptr);
                                shmdt(shm_info);
                                exit(EXIT_SUCCESS);
                            }

                            // [arbitrary] exit so that shared mem  // TODO send move to serverory is deleted properly
                            // exit(EXIT_SUCCESS);

                        }

                        else if (sscanf(current, "+ WA%s", server_placeholder) == 1){

                            snprintf(client_msg, MSGLEN, "OKWAIT\n");
                            sendClientMsg(socket_file_descriptor);
                        }

                        else if (sscanf(current, "+ MOVEO%s", server_placeholder) == 1) {

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
                shmdt(shmid_ptr);
                shmdt(shm_info);
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
    memset(client_msg, 0, MSGLEN);
}
