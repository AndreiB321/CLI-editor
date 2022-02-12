// Andrei Bulica 311CA
#include "image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NMAX 1000

int ascii_to_int(char *value)
{
	int x = 0, ok = 1;
	char *p = value;
	if (*p == '-') {
		ok = -1;
		p++;
	}
	while (*p) {
		x *= 10;
		x += (int)(*p - '0');
		p++;
	}
	return x * ok;
}

void set_value(int *x, int *y)
{
	if (*x > *y) {
		int swap_var = *x;
		*x = *y;
		*y = swap_var;
	}
}

RGB **memory_alloc(int n, int m)
{
	RGB **image = NULL;
	image = (RGB **)malloc(n * sizeof(RGB *));
	if (!image)
		return NULL;
	for (int i = 0; i < n; i++) {
		image[i] = (RGB *)malloc(m * sizeof(RGB));
		if (!image[i]) {
			// eliberare memorie anterior declarata
			while (--i >= 0)
				free(image[i]);
			free(image);
			return NULL;
		}
	}
	return image;
}

int minn(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

void modify_free(RGB **mat, int n)
{
	for (int i = 0; i < n; i++)
		free(mat[i]);
	free(mat);
}

void swap(RGB *a, RGB *b)
{
	RGB aux = *a;
	*a = *b;
	*b = aux;
}

int rotate90(images_d *data, int all_image)
{
	RGB **aux = NULL;
	aux = memory_alloc(data->x2 - data->x1, data->y2 - data->y1);
	if (!aux) {
		fprintf(stderr, "malloc() failed\n");
		return -1;
	}
	for (int i = 0; i < data->x2 - data->x1; ++i)
		for (int j = 0; j < data->y2 - data->y1; j++)
			aux[i][j] = data->image[data->y2 - 1 - j][data->x1 + i];

	if (all_image == 1) {
		// realocare matrice
		modify_free(data->image, data->y2 - data->y1);
		data->image = memory_alloc(data->x2 - data->x1, data->y2 - data->y1);
		if (!data->image) {
			fprintf(stderr, "malloc() failed\n");
			return -1;
		}
		for (int i = 0; i < data->x2 - data->x1; ++i)
			for (int j = 0; j < data->y2 - data->y1; j++)
				data->image[i][j] = aux[i][j];

		modify_free(aux, data->x2 - data->x1);
		return 0;
	}
	for (int i = 0; i < data->x2 - data->x1; ++i)
		for (int j = 0; j < data->y2 - data->y1; ++j)
			data->image[i + data->y1][j + data->x1] = aux[i][j];
	modify_free(aux, data->x2 - data->x1);
	return 0;
}

void set_image(images_d *data, int dim_x, int dim_y,
			   int x1, int y1, int x2, int y2)
{
	data->n = dim_x;
	data->m = dim_y;
	data->x1 = x1;
	data->y1 = y1;
	data->x2 = x2;
	data->y2 = y2;
}

int check_int(char *token)
{
	char *p = token;
	if (*p == '-')
		p++;
	for (; *p ; p++) {
		if (*p < '0' || *p > '9')
			return 0;
	}
	return -1;
}
