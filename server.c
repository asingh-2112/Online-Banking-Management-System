#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> 
#include "./ADMIN/ADMIN.h"
#include "./EMPLOYEE/employee_func.h"
// #include "./CUSTOMER.h"

void connection_handler(int connFd)
{
    printf("client is connected to the server\n");
    char rBuffer[1000], wBuffer[1000];
    ssize_t rBytes, wBytes;
    int customer[100];
    int employee[100];
    int manager[100];
    int choice;
    int exit=0;
        memset(wBuffer,0,sizeof(wBuffer));
    while(1){
        memset(rBuffer,0,sizeof(rBuffer));
        // memset(wBuffer,0,sizeof(wBuffer));
        strcat(wBuffer,"\nWelcome to bank!\nselect user\n1. Admin\n2. Manager\n3. Employee\n4. Customer\nPress any other number to exit\nEnter the number corresponding to the choice!");
        wBytes = write(connFd, wBuffer, sizeof(wBuffer));
        if (wBytes == -1)
            perror("Error while sending message to the user!");
        else
        {
            bzero(rBuffer, sizeof(rBuffer));
            rBytes = read(connFd, rBuffer, sizeof(rBuffer));
            if (rBytes == -1)
                perror("Error while reading from client");
            else if (rBytes == 0)
                printf("No data was sent by the client");
            else
            {
                choice = atoi(rBuffer);
                switch (choice)
                {
                case 1:
                    // Admin
                    memset(wBuffer,0,sizeof(wBuffer));
                    if(admin_operations(connFd))
                        strcpy(wBuffer,"Logged out as Admin\n");
                    break;
                case 2: //Manager
                memset(wBuffer,0,sizeof(wBuffer));
                    int res=employee_operations(connFd,true);
                    if(res==0){
                        strcpy(wBuffer,"Logged Out!\n");
                    }
                    if(res==1){
                        strcpy(wBuffer,"Manager ID doesn't exits. Attempts Over!\n");
                    }
                    if(res==2){
                        strcpy(wBuffer,"Entered wrong Password. Attempts Over!\n");
                    }
                    if(res==3){
                        strcpy(wBuffer,"Trying logging with different role. Try Again!\n");
                    }
                    break;

                case 3:
                    // Employee
                    memset(wBuffer,0,sizeof(wBuffer));
                    res=employee_operations(connFd,false);
                    if(res==0){
                        strcpy(wBuffer,"Logged Out!\n");
                    }
                    if(res==1){
                        strcpy(wBuffer,"Employee ID doesn't exits. Attempts Over!\n");
                    }
                    if(res==2){
                        strcpy(wBuffer,"Entered wrong Password. Attempts Over!\n");
                    }
                    if(res==3){
                        strcpy(wBuffer,"Trying logging with different role. Try Again!\n");
                    }
                    break;

                case 4: //Customer
                memset(wBuffer,0,sizeof(wBuffer));
                    res=customer_operations(connFd);
                    if(res==0){
                        strcpy(wBuffer,"Logged Out!\n");
                    }
                    else if(res==1){
                        strcpy(wBuffer,"Customer ID doesn't exits. Attempts Over!\n");
                    }
                    else if(res==2){
                        strcpy(wBuffer,"Customer wrong Password. Attempts Over!\n");
                    }
                    else if(res==3){
                        strcpy(wBuffer,"Your account is Deactivated \e[1m:(\e[m .Please visite your nearest bank branch.\n");
                    }
                    else if(res==-1){
                        strcpy(wBuffer,"Error While Logging In\n");

                    }
                    break;
                default:
                    // Exit
                    memset(wBuffer,0,sizeof(wBuffer));
                    break;
                }
            }
        }
        leave:
        if(exit) break;
    }
    printf("Terminating connection to client!\n");
}

void main()
{
    int socketFd, socketBindStatus, socketListenStatus, connFd;
    struct sockaddr_in serverAddress, clientAddress;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
    {
        perror("Error while creating server socket!");
        _exit(0);
    }
    int opt=1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(socketFd);
        _exit(1);
    }

    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_port = htons(8082);              // Server will listen to port 8080
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Binds the socket to all interfaces

    socketBindStatus = bind(socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (socketBindStatus == -1)
    {
        perror("Error while binding to server socket!");
        _exit(0);
    }

    socketListenStatus = listen(socketFd, 10);
    if (socketListenStatus == -1)
    {
        perror("Error while listening for connections on the server socket!");
        close(socketFd);
        _exit(0);
    }

    int clientSize;
    while (1)
    {
        clientSize = (int)sizeof(clientAddress);
        connFd = accept(socketFd, (struct sockaddr *)&clientAddress, &clientSize);
        if (connFd == -1)
        {
            perror("Error while connecting to client!");
            close(socketFd);
        }
        else
        {
            if (!fork())
            {
                // Child will enter this branch
                connection_handler(connFd);
                close(connFd);
                _exit(0);
            }
        }
    }

    close(socketFd);
}