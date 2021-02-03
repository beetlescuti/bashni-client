#include <sys/types.h>

#ifndef sharedMemory
#define sharedMemory

#define NAMELEN 1024
#define MAXPLAYERS 32
#define MAXTOWERLEN 25
#define BOARDSIZE 8

typedef struct {
    char game_name[NAMELEN];
    int our_playernum;
    int total_players;
    int max_moves;
    int total_pieces;
    char board[BOARDSIZE][BOARDSIZE][MAXTOWERLEN];
    int think_flag;
    pid_t thinker_id;
    pid_t connector_id;
} game_info;

typedef struct {
    int playernum;
    char name[NAMELEN];
    int flag; // is the player registered/bereit or not
} player_info;

typedef struct {
    game_info game_info;
    player_info all_players_info[MAXPLAYERS];
} all_info;

int create_shared_memory();
void struct_to_shared_memory(int shmid, game_info * info);

#endif
