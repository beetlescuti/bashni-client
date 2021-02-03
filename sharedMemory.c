#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>

#include "sharedMemory.h"

/* creates a game_info shared memory segment and returns the shmid */
int create_shared_memory(int num_players) {
    // create a new shared memory segment the size of our data structure
    int shmid = shmget(IPC_PRIVATE, sizeof(game_info) + sizeof(player_info[num_players]), IPC_CREAT | 0644);
    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return shmid;
}
