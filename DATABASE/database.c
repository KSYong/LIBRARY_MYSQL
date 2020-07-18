#include "database.h"

int main(){
	database_t* database = database_init();

	database_process_data(database);

	database_destroy(database);
}

database_t* database_init(){
	database_t* database = (database_t*)malloc(sizeof(database_t));
	if (database == NULL){
		perror("database malloc failed!!!");
		return -1;
	}
	database->mysql = mysql_init(NULL);
	if (database->mysql == NULL){
		perror("database->mysql initiation failed!!!");
		return -1;
	}
	if (mysql_real_connect(database->mysql, "localhost", "eric", "Eric0054!", "Books", 0, NULL, 0) == NULL){
		perror("mysql connection failed!!!");
		return -1;
	}
	database->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (database->sockfd < 0){
		perror("socket creation failed!!!");
		free(database);
		return -1;
	}

	memset(&database->database_addr, 0, sizeof(database->database_addr));
	memset(&database->server_addr, 0, sizeof(database->server_addr));

	database->database_addr.sin_family = AF_INET;
	database->database_addr.sin_port = htons(DATABASE_PORT);
	database->database_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(database->sockfd, (const struct sockaddr*)&database->database_addr, sizeof(database->database_addr))<0){
		perror("bind failed!!!");
		free(database);
		return -1;
	}
	return database;
}

void database_destroy(database_t* database){
	if (database){
		if(close(database->sockfd) == -1){
			perror("close failed!");
			return -1;
		}
		mysql_close(database->mysql);
		free(database);
	}
	else{
		perror("couldn't get database  object!");
		return -1;
	}
}

void database_process_data(database_t* database){
	if (database){
		ssize_t recv_byte;
		char buffer[BUF_MAX_LEN];
		const char* add = "add";
		const char* display = "display";
		const char* response = "command successfully executed.";
		socklen_t len = sizeof(database->server_addr);

		// recv data from server
		recv_byte = recvfrom(database->sockfd, (char *)buffer, BUF_MAX_LEN,MSG_WAITALL,(struct sockaddr*)&(database->server_addr), &len);
		
		if(recv_byte > 0){
			buffer[recv_byte] = '\0';
			printf("Client : %s\n", buffer);
			if(memcmp(buffer, add, sizeof("add")) == 0){
				database_add_data(database);
			}
			else if (memcmp(buffer, display, sizeof("display"))==0){
				database_display_data(database);
			}
		}
		else{
			perror("receiving data from client failed!!!");
		}
		// return data to server
		if(sendto(database->sockfd, (const char*)response, strlen(response), MSG_CONFIRM, (struct sockaddr*)&(database->server_addr), len) <= 0){
			perror("database to server data send failed!!!");
		}
		else{
			printf("Response sent from database to server\n");
		}
	}
	else{
		perror("couldn't get database  object!");
		return -1;
	}
}

void database_add_data(database_t* database){
	if(database){
		if(mysql_query(database->mysql, "INSERT INTO books VALUES(5, 'title6', 'author6')")){
			fprintf(stderr, "%s\n", mysql_error(database->mysql));
			mysql_close(database->mysql);
			return -1;
		}
		else{
			printf("Data added to DB\n");
		}
	}
	else{
		perror("couldn't get database object!");
		return -1;
	}
}

void database_display_data(database_t* database){
	if(database){
		if(mysql_query(database->mysql, "SELECT * FROM books")){
			fprintf(stderr, "%s\n", mysql_error(database->mysql));
			mysql_close(database->mysql);
			return -1;
		}
		else{
			printf("Data displayed from MySQL DB\n");
		}
		MYSQL_RES* result = mysql_store_result(database->mysql);
		int num_fields = mysql_num_fields(result);
		MYSQL_ROW row;

		while(row = mysql_fetch_row(result)){
			for(int i = 0; i < num_fields; i++){
				printf("%s ", row[i] ? row[i] : "NULL");
			}
			printf("\n");
		}
		mysql_free_result(result);
	}
	else{
		perror("couldn't get database object!");
		return -1;
	}
}
		
	

	
	
	


