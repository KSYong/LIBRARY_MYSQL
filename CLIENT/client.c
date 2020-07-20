#include "client.h"

/**
 * @fn int main(int argc, char** argv)
 * @brief client를 실행하기 위한 메인 함수 
 * @return int
 * @param argc 매개변수 개수
 * @param argv server에 전달할 명령어 
 */
int main(int argc, char **argv)
{
    client_t *client = client_init();
    if (argc == 2)
    {
        if (memcmp(argv[1], "display", sizeof("display")) == 0)
        {
            client_display_data(client);
        }
    }
    else if (argc == 4)
    {
        if (memcmp(argv[1], "add", sizeof("add")) == 0)
        {
            client_add_data(client, argv[2], argv[3]);
        }
    }
    client_destroy(client);
}

/**
 * @fn client_t* client_init()
 * @brief client 객체를 생성하고 초기화하는 함수
 * @return 생성 후 초기화된 client 객체
 */
client_t *client_init()
{
    client_t *client;
    if ((client = (client_t *)malloc(sizeof(client_t))) == NULL)
    {
        perror("Failed! malloc failure");
        return -1;
    }

    if ((client->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Failed! socket creation failure");
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
void client_destroy(client_t *client)
{
    if (client)
    {
        if (close(client->sockfd) == -1)
        {
            perror("Failed! socket close failure!");
        }
        free(client);
    }
    else
    {
        perror("Failed! client object pass failure");
        return -1;
    }
}

/**
 * @fn void client_add_data(client_t* client)
 * @brief server로 add 요청을 보내고 응답을 받는 함수
 * @return void
 * @param client 요청을 하기 위한 client 객체
 */
void client_add_data(client_t *client, char *title, char *author)
{
    if (client)
    {
        printf("Success! client requesting add command to server start\n");
        const char *add = "add";
        char buffer[BUF_MAX_LEN];
        ssize_t send_byte;
        ssize_t recv_byte;
        socklen_t len = sizeof(client->server_addr);
        //request command to server
        send_byte = sendto(client->sockfd, add, strlen(add), MSG_CONFIRM, (const struct sockaddr *)&(client->server_addr), len);
        if ((send_byte <= 0))
        {
            perror("Failed! client to server send failure");
            return -1;
        }
        else
        {
            printf("Success! add command sent from client to server\n");
            // 서버에 title 정보 보냄
            send_byte = sendto(client->sockfd, (const char*)title, strlen(title), MSG_CONFIRM, (const struct sockaddr *)&(client->server_addr), len);
            if ((send_byte <= 0))
            {
                perror("Failed! client to server send failure");
                return -1;
            }
            else
            {
                printf("Success! title sent from client to server\n");
            }
            // 서버에 author 정보 보냄
            send_byte = sendto(client->sockfd, (const char*)author, strlen(author), MSG_CONFIRM, (const struct sockaddr *)&(client->server_addr), len);
            if ((send_byte <= 0))
            {
                perror("Failed! client to server send failure");
                return -1;
            }
            else
            {
                printf("Success! author sent from client to server\n");
            }
        }
        //get success from server
        recv_byte = recvfrom(client->sockfd, (char *)buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr *)&(client->server_addr), &len);
        if (recv_byte > 0)
        {
            buffer[recv_byte] = '\0';
            printf("Server response : %s\n", buffer);
        }
        else
        {
            perror("Failed! receiving data from server failure");
            return -1;
        }
    }
}

/**
 * @fn void client_display_data(client_t* client)
 * @brief server로 display 명령을 보내고 display 정보를 받는 함수
 * @return void
 * @param client 요청을 하기 위한 client 객체
 */
void client_display_data(client_t *client)
{
    printf("Success! client requesting display command to server start\n");
    const char *display = "display";
    char buffer[BUF_MAX_LEN];
    ssize_t send_byte;
    ssize_t recv_byte;
    socklen_t len = sizeof(client->server_addr);
    //request display command to server
    send_byte = sendto(client->sockfd, display, strlen(display), 0, (struct sockaddr *)&(client->server_addr), len);
    if (send_byte <= 0)
    {
        perror("Failed! client to server send failure");
        return -1;
    }
    else
    {
        printf("Success! display command sent from client to server\n");
        //get display data from server
        recv_byte = recvfrom(client->sockfd, (char *)buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr *)&(client->server_addr), &len);
        if (recv_byte > 0)
        {
            printf("success! display information received from server\n");
            buffer[recv_byte] = '\0';
            int i = 0;
            int count = 0;
            while (buffer[i] != NULL)
            {
                printf("%c", buffer[i]);
                if (buffer[i] == ' ')
                {
                    count++;
                    if (count == 3)
                    {
                        printf("%c", buffer[i]);
                        printf("\n");
                        count = 0;
                    }
                }
                i++;
            }
        }
        else
        {
            perror("Failed! receiving data from server failure");
            return -1;
        }
    }
    //get success data from server
    recv_byte = recvfrom(client->sockfd, (char *)buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr *)&(client->server_addr), &len);
    if (recv_byte > 0)
    {
        buffer[recv_byte] = '\0';
        printf("Server response : %s\n", buffer);
    }
    else
    {
        perror("Failed! receiving data from server failure");
        return -1;
    }
}

void client_send_data(client_t *client, char *send_buf)
{
    ssize_t send_byte;
    socklen_t len = sizeof(client->server_addr);
    send_byte = sendto(client->sockfd, (const char *)send_buf, strlen(send_buf), MSG_CONFIRM, (const struct sockaddr *)&(client->server_addr), len);
    if ((send_byte <= 0))
    {
        perror("Failed! client to server send failure");
        return -1;
    }
    else
    {
        printf("Success! add command sent from client to server\n");
    }
}

char* client_recv_data(client_t *client){
    char recv_buf[BUF_MAX_LEN];
    ssize_t recv_byte;
    socklen_t len = sizeof(client->server_addr);
    recv_byte = recvfrom(client->sockfd, (char *)recv_buf, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr *)&(client->server_addr), &len);
    if (recv_byte > 0)
    {
        recv_buf[recv_byte] = '\0';
    }
    else
    {
        perror("Failed! receiving data from server failure");
        return -1;
    }
    return recv_buf;
}