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

int init_network(int *sockfd, int *connfd){
    
    if(host == 0)
    {
        printf("Initializing Network with client configuration\n");
        
        //Socket creation
        printf("Trying to create Socket .. \n");
        struct sockaddr_in servaddr, cli;
        *sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (*sockfd == -1) {
            printf("Socket creation failed.. \n");
            exit(0);
        }
        else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));
        
        
        //Assign IP and Port to STRUCT
        printf("Trying to assign IP address and port.. \n");
#ifdef LOCALHOST
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);
        printf("LOCALHOST and port successfully created.. \n");
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
        if (connect(*sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
            printf("connected to the server..\n");
        
        printf("Network is set up correctly");
        
    }
    else
    {
        printf("Initializing Network with host configuration\n");
        int len;
        //Socket creation
        printf("Trying to create Socket .. \n");
        struct sockaddr_in servaddr, cli;
        *sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (*sockfd == -1) {
            printf("Socket creation failed.. \n");
            exit(0);
        }
        else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));
        
        
        bzero(&servaddr, sizeof(servaddr));
        
        // Assigning IP and Port
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);
        
        //Binding Socket to Port
        if ((bind(*sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
            perror("Trying to bind socket..\n");
            return EXIT_FAILURE;
        }
        else
            printf("Socket successfully binded..\n");
        
        // Now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0) {
            printf("Listen failed...\n");
            exit(0);
        }
        else
            printf("Server listening..\n");
        len = sizeof(cli);
        
        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("server accept the client...\n");
        
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
            printf("You're joining a game.\n");
        }
    
    int socket, connfd;
    init_network(&socket, &connfd);
    system("clear");
    
    
    
    return 0;
}
