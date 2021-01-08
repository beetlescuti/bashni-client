#include<sys/types.h>

#define NAMELEN 128

#ifndef sharedMemory
#define sharedMemory

typedef struct {
    char game_name[NAMELEN];
    int our_player;
    int num_players;
    pid_t thinker_id;
    pid_t connector_id;
} game_info;

int create_shared_memory();
void struct_to_shared_memory(int shmid, game_info * info);

#endif

