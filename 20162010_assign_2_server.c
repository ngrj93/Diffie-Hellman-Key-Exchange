/* Author - @Nagaraj Poti
 * Roll no - 20162010
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define SERV_PORT 19000
#define MAX_LEN 1024
#define CAESAR_MOD 41	
#define LISTEN_Q 5

typedef struct GlobalInfo {
	int prime;
	int generator;
} GlobalInfo;

char caesar_encrypt(char c, int key) {
	char dict[] = {' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N',
								'O','P','Q','R','S','T','U','V','W','X','Y','Z',',','.',
								'?','0','1','2','3','4','5','6','7','8','9','!'};
	for (int i = 0; i < CAESAR_MOD; i++) {
		if (dict[i] == c) {
			return dict[(CAESAR_MOD + i - key) % CAESAR_MOD];
		}
	}
	return c;
}

/* Server main program */
int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Please enter command line arguments [IP_ADDRESS]\n");
		exit(-1);
	}
	printf("----------------------------------------------------------------\n");
	printf("SERVER\n");	
	printf("----------------------------------------------------------------\n");
	printf("Server started! Waiting for connection from the client...\n");

	GlobalInfo g;
	int server_sockfd, cli_sockfd;
	struct sockaddr_in serv_addr;
	memset(serv_addr, 0, sizeof(serv_addr));
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error: ");
		exit(-1);
	}
	
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);

	if (bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Error: ");
		exit(-1);
	}
	listen(server_sockfd, LISTEN_Q);

	while (true) {
		if ((cli_sockfd = accept(server_sockfd, NULL, NULL)) < 0) {
			perror("Error: ");
			exit(-1);
		}

		/* Receive key, generator and prime from client */
		int n = recv(cli_sockfd, buffer, MAX_LEN, 0);
	


		File *output;
		if((output = fopen("output.txt","w")) < 0) {
			perror("Error: ");
			exit(-1);
		}
		char buffer[MAX_LEN];
		memset(buffer, 0, sizeof(buffer));
		while ((n = recv(cli_sockfd, buffer, MAX_LEN, 0)) > 0) {
			for (int i = 0; i < n; i++) {
				char c = caesar_encrypt(buffer[i], caesar_key);
				putc(c, output);
			}
		}
	}

	return 0;
}

