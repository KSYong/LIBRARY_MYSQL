#pragma once

#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUF_MAX_LEN 1024 
#define CLIENT_PORT 7070
#define SERVER_PORT 8080
#define DATABASE_PORT 9090

/// @struct client_t
/// @brief server에게 메시지를 보내고 그 응답을 받기 위한 구조체
typedef struct client_s client_t;
struct client_s{
    /// client의 socket file descriptor
    int sockfd;
    /// server의 socket address
    struct sockaddr_in server_addr;
};

client_t* client_init();
void client_destroy(client_t* client);
void client_add_data(client_t* client, char* title, char* author);
void client_display_data(client_t* client);
void client_send_data(client_t *client, char *send_buf);
char* client_recv_data(client_t *client);
#endif
