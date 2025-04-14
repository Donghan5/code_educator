#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_complexity(void) {
	int i, j, k;

	while (i < 10000) {
		printf("i: %d\n", i);
		j = 0;
		while (j < 10000) {
			printf("j: %d\n", j);
			k = 0;
			while (k < 10000) {
				printf("k: %d\n", k);
				k++;
			}
			j++;
		}
		i++;
	}
}

int main(void) {

	printf("Welcome, to the complexity test!\n");
	test_complexity();

	return 0;
}
