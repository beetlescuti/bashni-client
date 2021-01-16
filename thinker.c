#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>

#include "thinker.h"
#include "sysprakclient.h"
#include "sharedMemory.h"
#include "printBoard.h"

int shmid_for_info;

void think(int * shmid_ptr) {
    // grab the actual shmid from shmid_ptr
    shmid_for_info = *shmid_ptr;

    // attach to shared memory
    all_info * rcv_info;
    rcv_info = (all_info*) shmat(shmid_for_info, NULL, 0);
    if (rcv_info == (void *) -1) {
    printf("Error attaching shared memory.\n");
        perror("shmat");
    }

    //check if think_flag was set by the connector process
    if (rcv_info->think_flag == 1) {
      //immediately set think_flag back to zero
      rcv_info->think_flag = 0;
    }

    // TODO: implement move thinker
    printf("thinking...\n");


    // TODO: send move back through pipe


    // detach from shared memory
    shmdt(shmid_ptr);
    shmdt(rcv_info);
}
