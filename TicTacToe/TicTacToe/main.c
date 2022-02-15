//
//  main.c
//  TicTacToe
//
//  Created by Jan Benecke on 15.02.22.
//

#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "host.h"
#include <string.h>

//Network
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SA struct sockaddr
#define PORT 8080
#define LOCALHOST

int host = 0;

int init_network(int *sockfd){
    
    if(host == 0)
    {
        printf("Initilizing Network with client configuration");
        
        //Socket creation
        printf("Trying to create Socket ..\n");
        struct sockaddr_in servaddr, cli;
        *sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (*sockfd == -1) {
            printf("Socket creation failed...\n");
            exit(0);
        }
        else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));
        
        
        //Assign IP and Port to STRUCT
        printf("Trying to assign IP address and port..\n");
#ifdef LOCALHOST
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);
        printf("LOCALHOST and port successfully created..\n");
#endif
#ifndef LOCALHOST
        
        printf("Please enter the ip address of your partner: (XXX.XXX.XXX.XXX)\n");
        char ip[15];
        { //IP ADDRESS
            char t;
            int i = 0;
            int br = 0;
            while (br != 0)
            {
                t = getchar();
                if(t != '\n')
                {
                    ip[i] = t;
                    i++;
                }
                else br = 1;
            };
        }
        
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(ip);
        servaddr.sin_port = htons(PORT);
        printf("IP and port successfully created..\n");
#endif
        
        // connect the client socket to server socket
        printf("Trying to connect to the host..\n");
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
            printf("connected to the server..\n");
        
        printf("Network is set up correctly");
        
    }
    
    
    for (int j; j<5; j++) {
        printf("Game starts in %i seconds", j);
        wait(1);
        system("clear");
    }
    return 1;
}

int main(int argc, const char * argv[]) {
    
    printf("Do you want to \"host\" or \"join\" a game? ");
    char inp_host[4];
    if (scanf("%s", &inp_host))
        if(strcmp(inp_host, "host") == 1)
            {
                host = 1;
                printf("You're hosting.");
            }
        else
        {
            printf("You're joining a game.");
        }
    
    int socket;
    init_network(&socket);
    
    
    
    
    system("clear");
    
    
    
    return 0;
}
