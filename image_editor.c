// Andrei Bulica 311CA
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "image.h"

#define NMAX 1000

int main(void)
{
	char op[NMAX] = "", *token, input[NMAX] = "";
	images_d data;
	int x;
	// initializare matrice
	data.image = NULL;
	set_image(&data, 0, 0, 0, 0, 0, 0);
	while (fgets(input, NMAX, stdin)) {
		// stocam tipul operatiei
		if (!strcmp(input, "\n")) {
			printf("Invalid command\n");
			continue;
		}
		get_operation(input, &token);
		strcpy(op, token);

		free((char *)token);
		if (strcmp(op, "EXIT") == 0) {
			if (!data.image)
				printf("No image loaded\n");
			break;
		}
		// executarea comenzii
		x = execute_operation(op, input, &data);
		// verificare probleme cu memoria
		if (x == -1)
			return -1;
	}
	if (data.image)
		modify_free(data.image, data.n);

	return 0;
}
