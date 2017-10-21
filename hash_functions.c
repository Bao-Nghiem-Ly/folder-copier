#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

char *hash(FILE *f) {
	char *hash_val = malloc(8 * sizeof(char));
	
	int x;
	for(x = 0; x < 8; x++){
		hash_val[x] = '\0';
	}
	
	char byte = '\0';
	int counter = 0;
	while (fread(&byte, 1, 1, f) == 1) {
		if (counter == 8) {
			counter = 0;
		}
		hash_val[counter] = hash_val[counter] ^ byte;
		counter++;
	}
	return hash_val;
}