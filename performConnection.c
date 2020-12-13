#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "performConnection.h"

int serverConnect(int socket_file_descriptor) {

    char server_msg[1024];

    while(1){

        if(recv(socket_file_descriptor, server_msg, 2000, 0) < 0) {
            printf("recv failed\n");
            break;
        }
        printf("S: %s", server_msg);
        break;

    }

    return 0;
}