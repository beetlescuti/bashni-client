#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "performConnection.h"

// TODO: wrap receive message (with newline as end) into function
// TODO: continue with implementation of protocol  
int serverConnect(int socket_file_descriptor) {

    char server_msg[1024];
    int result = 0;
    int packets = 0;
    int bytes_received= 0;
    memset(server_msg, 0, 1024);

    /* receive: is server accepting connections? */
    do {
        result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
        packets++;
        if (result > 0 ) {
            bytes_received+=result;
        }
        else {
            printf("Error: %i \n", result);
        }
        if (bytes_received >= 1024) {
            printf("Buffer overflow");
        break;
        }
    } while(server_msg[bytes_received-1] != '\n');

    // for debugging only
    // printf(  "S: %s bytes: %i packets: %i \n" , server_msg, bytes_received, packets);
    printf("S: %s" , server_msg);

    /* send: client version, major version must match!! */
    char client_version[14] = "VERSION 2.3\n";
    if (send(socket_file_descriptor, client_version, strlen(client_version), 0) < 0) {
        printf("send failed\n");
    } 
    else {
        printf("C: %s", client_version);
    }

    /* empty previous server message */
    result = 0;
    packets = 0;
    bytes_received= 0;
    memset(server_msg, 0, 1024);

    /* receive: is client version accepted? */
    do {
        result =recv(socket_file_descriptor, &server_msg[bytes_received], 1024-bytes_received, 0);
        packets++;
        if (result > 0 ) {
            bytes_received+=result;
        }
        else {
            printf("Error: %i \n", result);
        }
        if (bytes_received >= 1024) {
            printf("Buffer overflow");
            break;
        }
    } while(server_msg[bytes_received-1] != '\n');

    // for debugging only
    // printf(  "S: %s bytes: %i packets: %i \n" , server_msg, bytes_received, packets);
    printf("S: %s" , server_msg);

    return 0;
}