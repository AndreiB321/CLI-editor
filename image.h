// Andrei Bulica 311CA
#ifndef IMAGE
#define IMAGE

#include <stdio.h>

typedef struct {
	unsigned char r, b, g;
} RGB;

typedef struct {
	RGB **image;
	int n, m, x1, x2, y1, y2, type;
	unsigned char max_value;
} images_d;

void get_operation(const char *input, char **token);

int execute_operation(const char *operation, const char *input,
					  images_d *data);
int load_file(const char *input,  images_d *data);

int select_file(const char *input, images_d *data);

int rotate_file(const char *input, images_d *data);

int crop_file(images_d *data);

int grayscale_file(images_d *data);

int sepia_file(images_d *data);

int save_file(const char *input, images_d *data);

void get_dim(int *n, int *m);

int ascii_to_int(char *value);

void set_value(int *x, int *y);

RGB **memory_alloc(int n, int m);

int minn(int a, int b);

void modify_free(RGB **mat, int n);

int rotate90(images_d *data, int selectie_completa);

void swap(RGB *a, RGB *b);

int check_int(char *token);

void set_image(images_d *data, int dim_x, int dim_y,
			   int x1, int y1, int x2, int y2);
#endif
