#include "client.h"

/**
 * @fn int main(int argc, char** argv)
 * @brief client를 실행하기 위한 메인 함수 
 * @return int
 * @param argc 매개변수 개수
 * @param argv server에 전달할 명령어 
 */
int main(int argc, char** argv){
    client_t* client = client_init();
    if ( argc == 2){
		if (memcmp(argv[1], "add", sizeof("add"))==0){
            printf("client added\n");
            client_add_data(client);
        }
        else if (memcmp(argv[1], "display", sizeof("display"))==0){
            client_display_data(client);
        }
    }
    client_destroy(client);
}

/**
 * @fn client_t* client_init()
 * @brief client 객체를 생성하고 초기화하는 함수
 * @return 생성 후 초기화된 client 객체
 */
client_t* client_init(){    
    client_t* client = (client_t*)malloc(sizeof(client_t));

    if((client->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket creation failed!");
        free(client);
        return -1;
    }

    memset(&client->server_addr, 0, sizeof(client->server_addr));

    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(SERVER_PORT);
    client->server_addr.sin_addr.s_addr = INADDR_ANY;

    return client;
}

/**
 * @fn void client_destroy(client_t* client)
 * @brief client 객체를 삭제하기 위한 함수
 * @return void
 * @param client 삭제할 client 객체
 */
void client_destroy(client_t* client){
    if (client){
        if (close(client->sockfd) == -1){
        perror("close failed!");
        }
        free(client);
    }
    else{
        perror("client deliver to destroy function failed!");
        return -1;
    }
}

/**
 * @fn void client_add_data(client_t* client)
 * @brief server로 add 명령을 보내고 응답을 받는 함수
 * @return void
 * @param client 요청을 하기 위한 client 객체
 */
void client_add_data(client_t* client){
	if(client){
        socklen_t len = sizeof(client->server_addr);
   		const char *add = "add";
   		if((sendto(client->sockfd, add, strlen(add), MSG_CONFIRM, (const struct sockaddr*) &(client->server_addr), len)<=0)){
        	perror("client to server send failed!");
        	return -1;
    	}
    	else{
        	printf("add message sent from client to server\n");
    	}

    	char buffer[BUF_MAX_LEN];
    	ssize_t recv_byte;
    	recv_byte = recvfrom(client->sockfd, (char*) buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr*)&(client->server_addr), &len);
    	if(recv_byte > 0){
        	buffer[recv_byte] = '\0';
        	printf("Server : %s\n", buffer);
    	}
    	else{
        	perror("receiving data from server failed!!");
        	return -1;
    	}
	}
} 

/**
 * @fn void client_display_data(client_t* client)
 * @brief server로 display 명령을 보내고 응답을 받는 함수
 * @return void
 * @param client 요청을 하기 위한 client 객체
 */
void client_display_data(client_t* client){
    const char *display = "display";
    socklen_t len = sizeof(client->server_addr);
    if((sendto(client->sockfd, display, strlen(display), 0, (struct sockaddr*) &(client->server_addr), len)<=0)){
        perror("client to server send failed!");
        return -1;
    }
    else{
        printf("display message send from client to server\n");
    }

    char buffer[BUF_MAX_LEN];
    ssize_t recv_byte;
    recv_byte = recvfrom(client->sockfd, (char*) buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr*)&(client->server_addr), &len);
    if(recv_byte > 0){
        buffer[recv_byte] = '\0';
        printf("Server : %s\n", buffer);
    }
    else{
        perror("receiving data from server failed!!");
        return -1;
    }
}



