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
#define MAXSIZE 1000000
#define CAESAR_MOD 41

typedef struct GlobalInfo {
	int prime;
	int generator;
} GlobalInfo;

/* Function to compute (a ^ b) mod p */
int compute_exp_modulo(int a, int b, int p) {
	long long x = 1, y = a;
	while (b > 0) {
		if (b % 2 == 1)
			x = (x * y) % c;
		y = (y * y) % c;
		b /= 2;
	}
	return (int)(x % c);
}

/* Function to check primality of random generated numbers using Miller-Rabin Test */
int MillerRabinTest(int value, int iteration) {
	int q = value, k = 0;
	while (!(q % 2)) {
		q /= 2;
		k++;
	}
	for (int i = 0; i < iteration; i++) {
		int a = rand() % (value - 1) + 1;
		int current = q;
		bool flag = false;
		int mod_result = compute_exp_modulo(a, current, value);
		for (int i = 1; i < k; i++) {
			if (mod_result == 1 || mod_result == value - 1) {
				flag = true;
				break;
			}
			mod_result = (int)((long long)mod_result * mod_result % value);
		}
		if (flag)
			return false;
	}
	return true;
}

/* Generate a prime number that is going to be shared 
 * globally between client and server
 */
int GeneratePrime() {
	printf("Running Miller Rabin test on random numbers to ascertain primality...\n");
	srand(time(NULL));
	while(true) {
		int current_value = rand() % INT_MAX;
		if (!(current_value % 2))
			current_value++;
		if (MillerRabinTest(current_value, 10) == true)
			return current_value;
	}
}

/* Generate the primitive root by checking for random numbers */
int GeneratePrimitiveRoot(int p) {
	/* Construct sieve of primes */
	int sieve[MAXSIZE];
	memset(sieve, 0, sizeof(sieve));
	sieve[0] = sieve[1] = 1;
	for (int i = 4; i < MAXSIZE; i += 2)
		sieve[i] = 1;
	for (int i = 3; i < MAXSIZE; i += 2) {
		if (!sieve[i]) {
			for (int j = 2 * i; j < MAXSIZE; j += i)
				sieve[j] = 1;
		}
	}
	while (true) {
		int a = rand() % (p - 1) + 1;
		int phi = p - 1;
		bool flag = true;
		for (int i = 2; i < MAXSIZE; i++) {
			if (!sieve[i] && !(phi % 2)) {
				int mod_result = compute_exp_modulo(a, phi / i, p);
				if (mod_result == 1) {
					flag = false;
					break;
				}
			}
		}
		if (flag) 
			return a;
	}
}

/* Client main program */
int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Please enter command line arguments [IP_ADDRESS] [FILENAME]\n");
		exit(-1);
	}
	printf("---------------------------------------------------------------\n");
	printf("CLIENT\n");
	printf("---------------------------------------------------------------\n");
	printf("Client started! Establishing connection with the server...\n");

	/* Generate a prime number that is publicly known */
	GlobalInfo g;	
	g.prime = GeneratePrime();
	g.generator = GeneratePrimitiveRoot(g.prime);

	/* Establish socket connection with the server */
	int sockfd;
	struct sockaddr_in serv_addr;
	memset(serv_addr, 0, sizeof(serv_addr));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(SERV_PORT);
	
	/* Connect to the server */
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Error: ");
		exit(-1);
	}

	/* Choose a private key for the client */
	int private_key = rand() % (g.prime - 1) + 1;
	int public_key = compute_exp_modulo(g.generator, private_key, g.prime);

	/* Send public_key, generator and prime to the server */
	char message[MAX_LEN];
	memset(message, 0, sizeof(message));	 
	int n = sprintf(message, "%d\n%d\n%d\n", public_key, g.prime, g.generator);
	if (send(sockfd, message, n, 0) < 0) {
		perror("Error: ");
		exit(-1);
	}

	/* Receive server public key */
	n = recv(sockfd, message, MAX_LEN, 0);
	int server_public_key = atoi(message);

	int shared_key = compute_exp_modulo(server_public_key, private_key, g.prime);
	int caesar_key = shared_key % CAESAR_MOD;

	/* Send file contents to server after encryption */
	File *input;
	if ((input = fopen(argv[2], "r")) == NULL) {
		perror("Error: ");
		exit(-1);
	}
	while ((n = fread(message, sizeof(char), MAX_LEN, input)) > 0) {
		for (int i = 0; i < n; i++) 
			message[i] = caesar_encrypt(message[i], caesar_key);
		int n_sent = 0;
		while (n_sent != n) {
			int temp;
			if ((temp = send(sockfd, message + n_sent, n - n_sent, 0)) < 0) {
				perror("Error: ");
				exit(-1);
			}
			n_sent += temp;
		}
	}

	close(sockfd);

	printf("Finished sending the data to server!\n");
	return 0;
}



