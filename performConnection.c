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
//        char kick_off[10] = "Lets play";
//        if(send(socket_file_descriptor, kick_off, strlen(kick_off), 0) < 0){
//            printf("send failed\n");
//        }
//        else{
//            printf("C: %s\n", kick_off);
//    }


        int message_length = recv(socket_file_descriptor, server_msg, 1024, 0);

        /* check if server is accepting connections */
        if (message_length < 0) {
           printf("recv failed\n");
        }
        else {
          printf("Real server message from buffer: %s \n", server_msg);
        }

      //  while(strlen(server_msg) <= message_length){
        //     printf("%lu",strlen(server_msg));

          //   if ((strlen(server_msg)) == (message_length)){
            //   printf("S: %s real server message", server_msg);
            //    break; }}

        /* send client version:
          major version must match!! */
           char client_version[14] = "Version 2.2";
           if(send(socket_file_descriptor, client_version, strlen(client_version), 0) < 0){
               printf("send failed\n");
           }
           else{
             //    printf("Client version that we send: %s \n", client_version);
          }

           // empty server_msg to receive a new reply
        memset(server_msg, 0, 1024);
        printf("Server message should now be empty S: %s - Is it? \n", server_msg);

        /* check if client version is accepted */
        if(recv(socket_file_descriptor, server_msg, 1024, 0) < 0) {
            printf("recv failed\n");
       }
        else {
            printf("What we receive after sending our Version S: %s", server_msg);
        }

        printf("I will now sleep\n");
        usleep(5000000);


        return 0;
}