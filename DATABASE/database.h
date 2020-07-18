#pragma once

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <mysql.h>

#define BUF_MAX_LEN 1024
#define CLIENT_PORT 7070
#define SERVER_PORT 8080
#define DATABASE_PORT 9090


typedef struct database_s database_t;
struct database_s{
	MYSQL *mysql;
	int sockfd;
	struct sockaddr_in database_addr;
	struct sockaddr_in server_addr;
};

database_t* database_init();
void database_destroy(database_t* database);
void database_process_data(database_t* database);
void database_add_data(database_t* database);
void database_display_data(database_t* database);


#endif
