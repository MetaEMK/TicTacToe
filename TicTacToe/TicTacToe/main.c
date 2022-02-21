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
#define LOG_WIN "/W"
#define LOG_ACK "/AC"
#define LOG_START "/ST"
#define LOG_CON "/CO"

int host = 0;

char playground[3][3];
char inp_coor[3];
char didSomeoneWin = '-';

void preConfig(){
    for (int spalte = 0; spalte < 3; spalte++) {
        for (int zeile = 0; zeile < 3; zeile++) {
            playground[spalte][zeile] = '-';
        }
    }
}

void Output(){
    system("clear");
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            printf("%c  ", playground[row][col]);
        }
        printf("\n");
        
    }
}
int CheckWin(char a){
    int open=0;
    for (int row = 0; row <3; row++) {
        for (int col = 0; col<3; col++) {
            if (playground[row][col] == '-') {
                open = 1;
            }
        }
    }
    if (open == 0) didSomeoneWin = 'W';
    
    if (playground[0][0] == a && playground[1][1] == a && playground[2][2] == a) { didSomeoneWin = a; }
    if (playground[2][0] == a && playground[1][1] == a && playground[0][2] == a) { didSomeoneWin = a; }
    //Dia
    for (int i = 0; i < 3; i++) {
        //Row
        if (playground[i][0] == a && playground[i][1] == a && playground[i][2] == a) { didSomeoneWin = a; }
        //Col
        if (playground[0][i] == a && playground[1][i] == a && playground[2][i] == a) { didSomeoneWin = a; }
    }
    if (didSomeoneWin != '-') return 1;
    else return 0;
}
void Input(char *coord){
    int br1=1;
    int br2=1;
    char z = '0';
    char s = '0';
    while(br1 == 1 || br2 == 1){
        br1=1;
        br2=1;
        
        fflush(stdin);
        printf("Please enter coordinates row: 0/1/2\n");
        scanf("%c", &z);
        if(z == '0' || z == '1' || z == '2') {
            br1 = 0;
            
        } else printf("Wrong row input, please try again!\n");
        fflush(stdin);
        printf("Please enter coordinates column: 0/1/2 and column 0/1/2\n");
        scanf("%c", &s);
        if(s == '0' || s == '1' || s == '2') {
            br2 = 0;
        } else printf("Wrong column input, please try again!\n");
        fflush(stdin);
        
        int row = (int) z-48;
        int col = (int) s-48;
        if (playground[row][col] != '-')
        {
            printf("Wrong input, please try again!\n");
            br1=1;
        }
    }
    coord[1] =s;
    coord[0] =z;
}
void place_Input(char player, char *coord){
    playground[coord[0]-48][coord[1]-48] = player;
}


int client_game(int *connfd, int *sockfd){
    char txt[4];
    read(*sockfd, txt, sizeof(txt));
    write(*sockfd, LOG_ACK, sizeof(LOG_ACK));
    bzero(txt, 3);
    if (strcmp(txt, txt)!= 0) {
        printf("Game could not start!");
        return EXIT_FAILURE;
    }
    while (didSomeoneWin == '-') {
        read(*sockfd, txt, sizeof(txt));
        if (txt[2] == '-') {
            bzero(txt, sizeof(txt));
            read(*sockfd, txt, sizeof(txt)); //Gets Coords from P1
            place_Input('X', txt);
            Output();
            read(*sockfd, txt, sizeof(txt)); //Reads Con
            if (strcmp(txt, LOG_CON) == 0) {
                bzero(txt, sizeof(txt));
                Input(txt);//Asks for Action from 0
                place_Input('0', txt);
                write(*sockfd, txt, sizeof(txt));
                Output();
                printf("Please wait for player 1\n");
            }
        } else didSomeoneWin = txt[2];
    }
    if (didSomeoneWin == 'W') printf("Draw - Nobody won!\n");
    else printf("Player %c won\n",didSomeoneWin);
    return 0;
}

void write_message(char *mes){
    char t[] = LOG_WIN;
    int i=0;
    for (i=0; i<2; i++) mes[i] = t[i];
    mes[2] = didSomeoneWin;
}

int host_game(int *connfd, int *sockfd){
    char player;
    char txt[4];
    write(*connfd, LOG_START, sizeof(txt));
    read(*connfd, txt, sizeof(txt));
    if (strcmp(txt, LOG_ACK)!= 0) {
        printf("Game could not start!\n");
        return EXIT_FAILURE;
    }
    while (didSomeoneWin == '-') {
        write_message(txt);
        write(*connfd, txt, sizeof(txt));
        Output();
        player = 'X';
        Input(txt);
        place_Input(player, txt);
        Output();
        write(*connfd, txt, sizeof(txt)); //writes COORDS
        if(CheckWin(player) != 1)
        {
            printf("Please wait for player2\n");
            write(*connfd, LOG_CON, sizeof(LOG_CON));
            player = '0';
            read(*connfd, txt, sizeof(txt));
            place_Input(player, txt);
            Output();
            CheckWin(player);
        }
    }
    write_message(txt);
    write(*connfd, txt, sizeof(txt));
    if (didSomeoneWin == 'W') printf("Draw - Nobody won!\n");
    else printf("Player %c won\n",didSomeoneWin);
    return EXIT_SUCCESS;
}

int init_network(int *sockfd, int *connfd){
    
    if(host == 0)
    {
        printf("Initializing Network with client configuration\n");
        
        //Socket creation
        printf("Trying to create Socket .. \n");
        struct sockaddr_in servaddr;
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
        
        printf("Network is set up correctly\n");

    }
    else
    {
        printf("Initializing Network with host configuration\n");
        unsigned int len;
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
        if ((listen(*sockfd, 5)) != 0) {
            printf("Listen failed...\n");
            exit(0);
        }
        else
            printf("Server listening..\n");
        len = sizeof(cli);
        
        // Accept the data packet from client and verification
        *connfd = accept(*sockfd, (SA*)&cli, &len);
        if (*connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("server accept the client...\n");
        
        printf("Network is set up correctly\n");
        
    }

    return 1;
}

int main(int argc, const char * argv[]) {
    
    printf("Do you want to \"host\" or \"join\" a game? ");
    char inp_host[4];
    if (scanf("%s", inp_host))
        printf("INPUT: %s", inp_host);
        if(strcmp(inp_host, "host") == 0)
        {
            host = 1;
            printf("You're hosting.");
        }
        else printf("You're joining a game.\n");
    
    int socket, connfd;
    init_network(&socket, &connfd);
    preConfig();
    system("clear");
    if(host == 1) host_game(&connfd,&socket);
    else client_game(&connfd,&socket);
    
    
    close(socket);
    return 0;
}
