#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

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
 * globally between client and server */
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

/* Client main program */
int main(int argc, char *argv[]) {
	printf("---------------------------------------------------------------\n");
	printf("CLIENT\n");
	printf("---------------------------------------------------------------\n");
	printf("Client started! Establishing connection with the server...\n");

	/* Generate a prime number that is publicly known */
	GlobalInfo g;	
	g.prime = GeneratePrime();
		
