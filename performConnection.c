#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "performConnection.h"

int serverConnect(int socket_file_descriptor) {

    char server_msg[1024];

    while(1){
        /* check if server is accepting connections */
        if(recv(socket_file_descriptor, server_msg, 1024, 0) < 0) {
            printf("recv failed\n");
            break;
        }
        // TODO: do we need sleep? reply is too fast -> could be a different problem
        usleep(500000);
        printf("S: %s", server_msg);

        /* send client version:
           major version must match!! */
        char client_version[12] = "VERSION 2.1";
        if(send(socket_file_descriptor, client_version, strlen(client_version), 0) < 0){
            printf("send failed\n");
            break;
        }
        printf("C: %s\n", client_version);

        // empty server_msg to receive a new reply
        memset(server_msg, 0, 1024);

        /* check if client version is accepted */
        if(recv(socket_file_descriptor, server_msg, 1024, 0) < 0) {
            printf("recv failed\n");
            break;
        }
        printf("S: %s", server_msg);
        break;
    }

    return 0;
}