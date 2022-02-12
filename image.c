// Andrei Bulica 311CA
#include "image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NMAX 1000

void get_operation(const char *input, char **token)
{
	char op[NMAX] = "";
	strcpy(op, input);
	*token = strdup(strtok(op, "\n "));
}

int execute_operation(const char *operation, const char *input, images_d *data)
{
	if (!strcmp(operation, "LOAD"))
		return load_file(input, data);

	if (!strcmp(operation, "SELECT"))
		return select_file(input, data);

	if (!strcmp(operation, "ROTATE"))
		return rotate_file(input, data);

	if (!strcmp(operation, "CROP"))
		return crop_file(data);

	if (!strcmp(operation, "GRAYSCALE"))
		return grayscale_file(data);

	if (!strcmp(operation, "SEPIA"))
		return sepia_file(data);

	if (!strcmp(operation, "SAVE"))
		return save_file(input, data);

	printf("Invalid command\n");
	return 0;
}

int load_file(const char *input,  images_d *data)
{
	int dim_x, dim_y, file_type;
	char *token, space;
	unsigned char get_int;
	char op[NMAX], new_file[NMAX] = "./", s[] = "\n ", garbage[NMAX] = "";
	strcpy(op, input);
	token = strtok(op, s);
	token = strtok(NULL, s);
	strcat(new_file, token);
	FILE *in = fopen(new_file, "rb");
	if (!in) {
		modify_free(data->image, data->n);
		data->image = NULL;
		printf("Failed to load %s\n", token);
		return 0;
	}
	if (data->image)
		modify_free(data->image, data->n);
	fscanf(in, "%c", &space);
	if (space == '#')
		fgets(garbage, NMAX, in);
	else
		fseek(in, -1, SEEK_CUR);
	fscanf(in, "%c%d\n", &space, &file_type);
	fscanf(in, "%c", &space);
	if (space == '#')
		fgets(garbage, NMAX, in);
	else
		fseek(in, -1, SEEK_CUR);
	fscanf(in, "%d%d\n", &dim_y, &dim_x);
	set_image(data, dim_x, dim_y, 0, 0, dim_y, dim_x);
	data->image = memory_alloc(dim_x, dim_y);
	if (!data->image) {
		fprintf(stderr, "malloc() failed\n");
		return -1;
	}
	data->type = file_type;
	if (file_type != 1 && file_type != 4) {
		fscanf(in, "%c", &space);
		if (space == '#')
			fgets(garbage, NMAX, in);
		else
			fseek(in, -1, SEEK_CUR);
		fscanf(in, "%hhu\n", &data->max_value);
	}
	if (file_type < 4) {
		for (int i = 0; i < data->n ; i++) {
			for (int j = 0; j < data->m; j++) {
				if (file_type == 3) {
					fscanf(in, "%hhu", &data->image[i][j].r);
					fscanf(in, "%hhu", &data->image[i][j].g);
					fscanf(in, "%hhu", &data->image[i][j].b);
				} else {
					fscanf(in, "%hhu", &get_int);
					data->image[i][j].r = get_int;
					data->image[i][j].g = get_int;
					data->image[i][j].b = get_int;
				}
			}
		}
	} else {
		for (int i = 0; i < data->n ; i++) {
			for (int j = 0; j < data->m; j++) {
				if (file_type == 6) {
					fread(&data->image[i][j].r, sizeof(unsigned char), 1, in);
					fread(&data->image[i][j].g, sizeof(unsigned char), 1, in);
					fread(&data->image[i][j].b, sizeof(unsigned char), 1, in);
				} else {
					fread(&get_int, sizeof(unsigned char), 1, in);
					data->image[i][j].r = get_int;
					data->image[i][j].g = get_int;
					data->image[i][j].b = get_int;
				}
			}
		}
	}
	fclose(in);
	printf("Loaded %s\n", token);
	return 0;
}

int select_file(const char *input, images_d *data)
{
	if (data->image) {
		char *token, s[] = "\n ", op[NMAX];
		strcpy(op, input);
		int i = 0, ok = 0, v[10] = {0};
		token = strtok(op, s);
		while (token) {
			token = strtok(NULL, s);
			if (!token)
				break;
			if (strcmp(token, "ALL") == 0) {
				v[0] = 0;
				v[1] = 0;
				v[2] = data->m;
				v[3] = data->n;
				ok = 1;
				break;
			}
			if (!check_int(token)) {
				printf("Invalid command\n");
				return 0;
			}
			v[i++] = ascii_to_int(token);
		}
		if (i != 4 && !ok) {
			printf("Invalid command\n");
			return 0;
		}
		// aranjare parametrii
		set_value(&v[0], &v[2]);
		set_value(&v[1], &v[3]);
		// verificare parametrii
		if (v[2] > data->m || v[3] > data->n ||
			v[0] < 0 || v[1] < 0 || v[2] == v[0] || v[1] == v[3]) {
			printf("Invalid set of coordinates\n");
			return 0;
		}
		// setare parametrii
		set_image(data, data->n, data->m, v[0], v[1], v[2], v[3]);
		if (!ok)
			printf("Selected %d %d %d %d\n", data->x1,
				   data->y1, data->x2, data->y2);
		else
			printf("Selected ALL\n");

	} else {
		printf("No image loaded\n");
	}
	return 0;
}

int rotate_file(const char *input,  images_d *data)
{
	if (data->image) {
		int check;
		char *token = NULL, s[] = "\n ", op[NMAX] = "";
		strcpy(op, input);
		int ok = 0, x;
		token = strtok(op, s);
		if (token) {
			token = strtok(NULL, s);
			x = ascii_to_int(token);
		}
		if (x == 0 || x == -360 || x == 360) {
			printf("Rotated %d\n", x);
			return 0;
		}
		if (x < 0) {
			x += 360;
			ok = 1;
		}
		if (x % 90 != 0 || x == 0) {
			printf("Unsupported rotation angle\n");
			return 0;
		}
		if (data->x2 - data->x1 == data->m && data->y2 - data->y1 == data->n) {
			// aplicare alogritm de rotire
			for (int i = 0; i < x / 90; i++) {
				check = rotate90(data, 1);
				if (check)
					return -1;
				set_image(data, data->x2 - data->x1,
						  data->y2 - data->y1, 0, 0,
						  data->y2 - data->y1, data->x2 - data->x1);
			}
			printf("Rotated ");
			if (ok)
				printf("%d\n", x - 360);
			else
				printf("%d\n", x);
			return 0;
		}
		if (data->x2 - data->x1 != data->y2 - data->y1) {
			printf("The selection must be square\n");
			return 0;
		}
		// aplicare alogritm de rotire
		for (int i = 0; i < x / 90; i++)
			check = rotate90(data, 0);
		if (check)
			return -1;

		printf("Rotated ");
		if (ok)
			printf("%d\n", x - 360);
		else
			printf("%d\n", x);
	} else {
		printf("No image loaded\n");
	}
	return 0;
}

int crop_file(images_d *data)
{
	if (data->image) {
		// alocare matrice auxiliara
		RGB **aux = NULL;
		aux = memory_alloc(data->y2 - data->y1, data->x2 - data->x1);
		if (!aux) {
			fprintf(stderr, "malloc() failed\n");
			return -1;
		}
		// copiere suprafata
		for (int i = data->y1; i < data->y2; i++)
			for (int j = data->x1; j < data->x2; j++)
				aux[i - data->y1][j - data->x1] = data->image[i][j];
		// eliberare memorie matrice initiala
		modify_free(data->image, data->n);
		data->m = data->x2 - data->x1;
		data->n = data->y2 - data->y1;
		data->image = memory_alloc(data->n, data->m);
		if (!data->image) {
			fprintf(stderr, "malloc() failed\n");
			return -1;
		}
		// copiere matrice salvata
		for (int i = 0; i < data->y2 - data->y1; i++)
			for (int j = 0; j < data->x2 - data->x1; j++)
				data->image[i][j] = aux[i][j];

		modify_free(aux, data->y2 - data->y1);
		aux = NULL;
		set_image(data, data->n, data->m, 0, 0, data->m, data->n);
		printf("Image cropped\n");

	} else {
		printf("No image loaded\n");
	}
	return 0;
}

int grayscale_file(images_d *data)
{
	if (data->image) {
		if (data->type != 3 && data->type != 6) {
			printf("Grayscale filter not available\n");
			return 0;
		}
		double sum;
		for (int i = data->y1; i < data->y2; i++) {
			for (int j = data->x1; j < data->x2; j++) {
				sum = (data->image[i][j].r + data->image[i][j].g
					+ data->image[i][j].b) / 3;
				data->image[i][j].r = (unsigned char)sum;
				data->image[i][j].g = (unsigned char)sum;
				data->image[i][j].b = (unsigned char)sum;
			}
		}
		printf("Grayscale filter applied\n");
	} else {
		printf("No image loaded\n");
	}
	return 0;
}

int sepia_file(images_d *data)
{
	if (data->image) {
		if (data->type != 3 && data->type != 6) {
			printf("Sepia filter not available\n");
			return 0;
		}
		double new_red, new_green, new_blue;
		for (int i = data->y1; i < data->y2; i++) {
			for (int j = data->x1; j < data->x2; j++) {
				new_red = 0.5 + 0.393 * data->image[i][j].r
						+ 0.769 * data->image[i][j].g
						+ 0.189 * data->image[i][j].b;
				new_green = 0.5 + 0.349 * data->image[i][j].r
						+ 0.686 * data->image[i][j].g
						+ 0.168 * data->image[i][j].b;
				new_blue = 0.5 + 0.272 * data->image[i][j].r
						+ 0.534 * data->image[i][j].g
						+ 0.131 * data->image[i][j].b;
				data->image[i][j].r = minn(new_red, data->max_value);
				data->image[i][j].g = minn(new_green, data->max_value);
				data->image[i][j].b = minn(new_blue, data->max_value);
			}
		}
		printf("Sepia filter applied\n");
	} else {
		printf("No image loaded\n");
	}
	return 0;
}

int save_file(const char *input,  images_d *data)
{
	if (!data->image) {
		printf("No image loaded\n");
		return 0;
	}
	char *token, name_file[NMAX] = "", ascii[NMAX] = "",
			s[] = "\n ", new_file[NMAX] = "./";
	char op[NMAX] = "";
	int cnt = 0;
	strcpy(op, input);
	token = strtok(op, s);
	token = strtok(NULL, s);
	while (token) {
		if (cnt == 0) {
			strcpy(name_file, token);
			strcat(new_file, name_file);
			cnt++;
		} else {
			strcpy(ascii, token);
		}
		token = strtok(NULL, s);
	}
	FILE *out = fopen(new_file, "wb");
	if (strcmp(ascii, "ascii") != 0) {
		if (out) {
			fprintf(out, "%c", 'P');
			if (data->type <= 3)
				data->type += 3;
			fprintf(out, "%d\n%d %d\n", data->type, data->m, data->n);
			if (data->type != 4)
				fprintf(out, "%d\n", data->max_value);
			for (int i = 0; i < data->n; i++) {
				for (int j = 0; j < data->m; j++) {
					if (data->type != 6) {
						fwrite(&data->image[i][j].r,
							   sizeof(char), 1, out);
					} else {
						fwrite(&data->image[i][j].r, sizeof(char), 1, out);
						fwrite(&data->image[i][j].g, sizeof(char), 1, out);
						fwrite(&data->image[i][j].b, sizeof(char), 1, out);
					}
				}
			}
		}
	} else {
		if (out) {
			fprintf(out, "P");
			if (data->type > 3)
				data->type -= 3;
			fprintf(out, "%d\n%d %d\n", data->type, data->m, data->n);
			if (data->type != 1)
				fprintf(out, "%d\n", data->max_value);
			for (int i = 0; i < data->n; i++) {
				for (int j = 0; j < data->m; j++) {
					if (data->type != 3) {
						fprintf(out, "%d ", data->image[i][j].r);
					} else {
						fprintf(out, "%d ", data->image[i][j].r);
						fprintf(out, "%d ", data->image[i][j].g);
						fprintf(out, "%d ", data->image[i][j].b);
					}
				}
				fprintf(out, "\n");
			}
		}
	}
	fclose(out);
	printf("Saved %s\n", name_file);
	return 0;
}
