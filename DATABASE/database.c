#include "database.h"

/**
 * @fn int main()
 * @brief database 실행을 위한 메인 함수
 * @return int
 */
int main()
{
	database_t *database = database_init();

	database_process_data(database);

	database_destroy(database);
}

/**
 * @fn database_t* databse_init()
 * @brief  database 객체를 생성하고 초기화하는 함수
 * @return 생성된 database 객체
 */
database_t* database_init()
{
	database_t *database = (database_t *)malloc(sizeof(database_t));
	if(database == NULL){
		perror("Failed! database malloc failure");
		return -1;
	}
	database->mysql = mysql_init(NULL);
	if (database->mysql == NULL)
	{
		perror("Failed! database.mysql initiation failed!!!");
		return -1;
	}
	if (mysql_real_connect(database->mysql, "localhost", "eric", "pw123", "Books", 0, NULL, 0) == NULL)
	{
		perror("Failed! mysql connection failed!!!");
		return -1;
	}
	database->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (database->sockfd < 0)
	{
		perror("Failed! socket creation failed!!!");
		free(database);
		return -1;
	}

	memset(&database->database_addr, 0, sizeof(database->database_addr));
	memset(&database->server_addr, 0, sizeof(database->server_addr));

	database->database_addr.sin_family = AF_INET;
	database->database_addr.sin_port = htons(DATABASE_PORT);
	database->database_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(database->sockfd, (const struct sockaddr *)&database->database_addr, sizeof(database->database_addr)) < 0)
	{
		perror("Failed! bind failed!!!");
		free(database);
		return -1;
	}
	return database;
}

/**
 * @fn void database_destroy(database_t* dtaabase)
 * @brief database 객체를 삭제하기 위한 함수
 * @return void
 * @param database 삭제할 database 객체
 */
void database_destroy(database_t *database)
{
	if (database)
	{
		if (close(database->sockfd) == -1)
		{
			perror("Failed! close failed!");
			return -1;
		}
		mysql_close(database->mysql);
		free(database);
	}
	else
	{
		perror("Failed! couldn't get database  object!");
		return -1;
	}
}

/**
 * @fn void database_process_data(database_t* database)
 * @brief server에서 요청된 명령에 따라 mysql에 요청하고 그 결과를 server에 반환하는 함수
 * @return void
 * @param database 요청과 반환을 수행할 database 객체
 */
void database_process_data(database_t *database)
{
	if (database)
	{
		ssize_t send_byte;
		ssize_t recv_byte;
		char buffer[BUF_MAX_LEN];
		const char *add = "add";
		const char *display = "display";
		const char *response = "command successfully executed.";
		socklen_t len = sizeof(database->server_addr);

		// get command from server 
		recv_byte = recvfrom(database->sockfd, (char *)buffer, BUF_MAX_LEN, MSG_WAITALL, (struct sockaddr *)&(database->server_addr), &len);

		if (recv_byte > 0)
		{
			buffer[recv_byte] = '\0';
			printf("Client request : %s\n", buffer);
			if (memcmp(buffer, add, sizeof("add")) == 0)
			{
				database_add_data(database);
			}
			else if (memcmp(buffer, display, sizeof("display")) == 0)
			{
				database_display_data(database);
			}
		}
		else
		{
			perror("Failed! receiving data from client failure");
		}
		// return success data to server
		send_byte = sendto(database->sockfd, (const char *)response, strlen(response), MSG_CONFIRM, (struct sockaddr *)&(database->server_addr), len);
		if (send_byte <= 0)
		{
			perror("Failed! database to server data send failure");
		}
		else
		{
			printf("Success! Response sent from database to server\n");
		}
	}
	else
	{
		perror("Failed! couldn't get database  object!");
		return -1;
	}
}

/**
 * @fn void database_add_data(database_t* database)
 * @brief mysql에 row 추가 요청을 보내는 함수
 * @return void
 * @param database 작업을 수행할 database 객체
 */
void database_add_data(database_t *database)
{
	if (database)
	{
		printf("Success! database add start\n");
		if (mysql_query(database->mysql, "INSERT INTO Books VALUES(NULL, 'title', 'author')"))
		{
			fprintf(stderr, "%s\n", mysql_error(database->mysql));
			mysql_close(database->mysql);
			return -1;
		}
		else
		{
			printf("Success! Data added to DB\n");
		}
	}
	else
	{
		perror("Failed! couldn't get database object!");
		return -1;
	}
}

/**
 * @fn void database_display_data(database_t* database)
 * @brief mysql에 Books 테이블 정보를 요청하고 결과를 받아 서버에 보내는 함수
 * @return void
 * @param database 작업을 수행할 database 객체
 */
void database_display_data(database_t *database)
{
	if (database)
	{
		char buf[50][0];
		char send_buf[200] = {0};
		if (mysql_query(database->mysql, "SELECT * FROM Books"))
		{
			fprintf(stderr, "%s\n", mysql_error(database->mysql));
			mysql_close(database->mysql);
			return -1;
		}
		else
		{
			printf("Succes! Data displayed from MySQL DB\n");
		}
		MYSQL_RES *result = mysql_store_result(database->mysql);
		int num_fields = mysql_num_fields(result);
		MYSQL_ROW row;
		int j = 0;
		while (row = mysql_fetch_row(result))
		{
			for (int i = 0; i < num_fields; i++)
			{
				if (row[i])
				{
					sprintf(&buf[j][0], "%s ", row[i]);
					printf("%s", buf[j]);
					strcat(send_buf, buf[j]);
					
				}
				else
				{ 
					printf("NULL");
				}
			}
			printf("\n");
			j++;
		}
		mysql_free_result(result);
		ssize_t send_byte;
		socklen_t len = sizeof(database->server_addr);
		//send display data to server
		send_byte = sendto(database->sockfd, (const char *)send_buf, strlen(send_buf), MSG_CONFIRM, (struct sockaddr *)&(database->server_addr), len);
		if (send_byte <= 0)
		{
			perror("Failed! database to server data send failure");
		}
		else
		{
			printf("Success! Display data  sent from database to server\n");
		}
	}
	else
	{
		perror("Failed! couldn't get database object!");
		return -1;
	}
}
