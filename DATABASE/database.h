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

/// @struct database_t
/// @brief server에서 받은 요청을 처리하는 database 구조체 
typedef struct database_s database_t;
struct database_s{
	///  mysql 연결을 위한 MYSQL 구조체
	MYSQL *mysql;
	/// 소켓 파일 디스크립터
	int sockfd;
	/// 데이터베이스 소켓 주소
	struct sockaddr_in database_addr;
	/// 서버 소켓 주소
	struct sockaddr_in server_addr;
};

database_t* database_init();
void database_destroy(database_t* database);
void database_process_data(database_t* database);
void database_add_data(database_t* database);
void database_display_data(database_t* database);

#endif
