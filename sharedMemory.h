#include<sys/types.h>

#define NAMELEN 1024

#ifndef sharedMemory
#define sharedMemory

typedef struct {
    char game_name[NAMELEN];
    int our_playernum;
    int total_players;
    pid_t thinker_id;
    pid_t connector_id;
} game_info;

typedef struct {
    int playernum;
    char name[NAMELEN];
    int flag; // is the player registered/bereit or not
} player;

int create_shared_memory();
void struct_to_shared_memory(int shmid, game_info * info);

#endif

