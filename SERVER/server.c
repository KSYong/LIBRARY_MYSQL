#include "server.h"

/**
 * @mainpage Project : LIBRARY_MYSQL
 * @section intro 소개
 *              -UDP 와 MySQL을 이용한 도서 관리 프로그램입니다
 * @section CreateInfo 작성 정보
 *              -작성자 : 권승용
 *              -작성일 : 2020/07/13
 * @subsection exec 실행 방법 및 인수 설명
 *              -실행 방법\n
 *                  서버    : SERVER/server
 *                  클라이언트 : CLIENT/client
 */

/**
 * @fn int main()
 * @brief server 실행을 위한 메인 함수
 * @return int
 */
int main(){
    server_t* server = server_init();

    server_process_data(server);

    server_destroy(server);
}

/**
 * @fn server_t* server_init()
 * @brief server 객체를 생성하고 초기화하는 함수
 * @return 생성 후 초기화된 server 객체
 */
server_t* server_init(){
    server_t* server = (server_t*)malloc(sizeof(server_t));

	int fd = 0;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    if((fd < 0){
        perror("socket creation failed!");
        free(server);
        return -1;        
    }
	else{
		server->sockfd = fd;
	}

    memset(&server->database_addr, 0, sizeof(server->database_addr));
    memset(&server->server_addr, 0, sizeof(server->server_addr));
    memset(&server->client_addr, 0, sizeof(server->client_addr));
	
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(SERVER_PORT);
    server->server_addr.sin_addr.s_addr = INADDR_ANY;
	
    server->database_addr.sin_family = AF_INET;
    server->database_addr.sin_port = htons(DATABASE_PORT);
    server->database_addr.sin_addr.s_addr = INADDR_ANY;

	int rv = 0;
	rv = bind(server->sockfd, (const struct sockaddr*)&server->server_addr, sizeof(server->server_addr));
    if( rv  < 0 ){
        perror("bind failed!");
        free(server);
        return -1;
    }
    return server;
}

/**
 * @fn void server_destroy(server_t* server)
 * @brief server 객체를 삭제하기 위한 함수
 * @return void
 * @param server 삭제할 server 객체
 */
void server_destroy(server_t* server){
    if (server){
		int rv = 0;
		rv = close(server->sockfd);
        if (rv  == -1){
            perror("close failed!");
            return -1;
        }
        free(server);
    }
    else{
        perror("couldn't get server object!");
        return -1;
    }
}

/**
 * @fn void server_process_data(server_t* server)
 * @brief client에서 오는 명령에 따라 mysql에 요청하고 응답을 받아 다시 client에 응답하는 함수
 * @return void
 * @param server 명령을 처리할 server 객체
 */
void server_process_data(server_t* server){
    if(server){
        ssize_t recv_byte;
        char buffer[BUF_MAX_LEN];
        const char* add = "add";
        const char* display = "display";
        const char* response = "Command successfully executed.";
        socklen_t len =  sizeof(server->client_addr);

        //client에서 명령 받음 
        recv_byte = recvfrom(server->sockfd, (char *)buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr*) &(server->client_addr), &len);
        if (recv_byte > 0){
            buffer[recv_byte] = '\0';
            printf("Client : %s\n", buffer);
            if (memcmp(buffer, add, sizeof("add")) == 0){
                //database로 요청 시작
                server_add_data(server);
            }
            else if (memcmp(buffer, display, sizeof("display"))==0 ){
                server_display_data(server);
            }
        }
        else{
            perror("receiving data from client failed!!!");
        }
        //client로 응답 보냄 
        if(sendto(server->sockfd, (const char*)response, strlen(response),  MSG_CONFIRM, (struct sockaddr*)&(server->client_addr), len) <= 0){
            perror("server to client data send failed!!!");
        }
        else{
            printf("Response sent from server to client\n");
        }   
    }
    else{
        perror("couldn't get server object!");
        return -1;
    }
}

/**
 * @fn void server_add_data(server_t* server)
 * @brief MySQL의 books 테이블에 데이터 추가를 요청하는 함수
 * @return void
 * @param server 데이터 추가를 요청할 server 객체
 */
void server_add_data(server_t* server){
    //database로 요청 시작 
    ssize_t send_byte;
    ssize_t recv_byte;
    const char* add = "add";
    char buffer[BUF_MAX_LEN];
    socklen_t len = sizeof(server->database_addr);
    if (server){
        send_byte = sendto(server->sockfd, (const char*)add, strlen(add), MSG_CONFIRM, (struct sockaddr*)&(server->database_addr), len);
        if(send_byte > 0){
            printf("request sent from server to database\n");
            printf("server to database send success\n");
		}
		else{
            perror("server to database data send failed!!!");
            return -1;
		}
        recv_byte = recvfrom(server->sockfd, (char *)buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr*) &(server->database_addr), &len);
        if(recv_byte > 0){
            buffer[recv_byte] = '\0';
            printf("Database : %s\n", buffer);
            printf("Return from Database to Server confirmed!\n");
        }
        else{
            perror("Return from database to server failed!");
            return -1;
        }
    }
     else{
        perror("couldn't get server object!");
        return -1;
    }
}

/**
 * @fn void server_display_data(server_t* server)
 * @brief MySQL의 books 테이블의 모든 데이터들을 표시하는 요청을 보내는 함수
 * @return void
 * @param server MySQL에 요청을 보낼  server 객체
 */
void server_display_data(server_t* server){
    if (server){
		if(sendto(server->sockfd, (const char*)display, strlen(display), MSG_CONFIRM, (struct sockaddr*)&(server->client_addr), len) <= 0){
			perror("server to database data send failed!
}
