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

    server->mysql = mysql_init(NULL);
    
    if (mysql_real_connect(server->mysql, "localhost", "eric", "Eric0054!", "Books", 0, NULL, 0)==NULL){
        perror("mysql connection failed!!");
        return -1;
    }

    if((server->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket creation failed!");
        free(server);
        return -1;        
    }

    memset(&server->server_addr, 0, sizeof(server->server_addr));
    memset(&server->client_addr, 0, sizeof(server->client_addr));

    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(PORT);
    server->server_addr.sin_addr.s_addr = INADDR_ANY;

    if( bind(server->sockfd, (const struct sockaddr*)&server->server_addr, sizeof(server->server_addr)) < 0 ){
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
        if (close(server->sockfd) == -1){
            perror("close failed!");
            return -1;
        }
        mysql_close(server->mysql);
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
        const char * add = "add";
        const char * display = "display";
        const char *response = "Command successfully executed.";
        socklen_t len =  sizeof(server->client_addr);

        recv_byte = recvfrom(server->sockfd, (char *)buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr*) &(server->client_addr), &len);
        if (recv_byte > 0){
            buffer[recv_byte] = '\0';
            printf("Client : %s\n", buffer);
            if (memcmp(buffer, add, sizeof("add")) == 0){
                server_add_data(server);
            }
            else if (memcmp(buffer, display, sizeof("display"))==0 ){
                server_display_data(server);
            }
        }
        else{
            perror("receiving data from client failed!!!");
        }
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
    if (server){
        if (mysql_query(server->mysql, "INSERT INTO books VALUES(3, 'title4', 'author4')")){
            fprintf(stderr, "%s\n", mysql_error(server->mysql));
            mysql_close(server->mysql);
            return -1;
        }
        else{
            printf("Data added to DB\n");
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
        if(mysql_query(server->mysql, "SELECT * FROM books")){
            fprintf(stderr, "%s\n", mysql_error(server->mysql));
            mysql_close(server->mysql);
            return -1;
        }
        else{
            printf("Data displayed from DB\n");
        }

        MYSQL_RES *result = mysql_store_result(server->mysql);
        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;

        while(row = mysql_fetch_row(result)){
            for(int i = 0; i < num_fields; i++){
                printf("%s  ", row[i] ? row[i] : "NULL");
            }
            printf("\n");
        }
        mysql_free_result(result);
    }
    else{
        perror("couldn't get server object!");
        return -1;
    }
}