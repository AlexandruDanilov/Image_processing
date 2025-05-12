#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_editor_functions.h"

void handle_ascii_save(FILE *file, image_t *image)
{
	// Save according to format
	if (strcmp(image->type, "P2") == 0 || strcmp(image->type, "P3") == 0) {
		fprintf(file, "%s\n%d %d\n%d\n", image->type, image->width,
				image->height, image->maxval);
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				if (j > 0) {
					fprintf(file, " ");
				}
				if (strcmp(image->type, "P3") == 0) {
					// RGB
					fprintf(file, "%d %d %d", image->data[i][j * 3],
							image->data[i][j * 3 + 1],
							image->data[i][j * 3 + 2]);
				} else {
					// greyscale
					fprintf(file, "%d", image->data[i][j]);
				}
			}
			fprintf(file, "\n");
		}
	} else if (strcmp(image->type, "P5") == 0) {
		// "Convert" P5 to P2
		fprintf(file, "P2\n%d %d\n%d\n", image->width,
				image->height, image->maxval);
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				if (j > 0) {
					fprintf(file, " ");
				}
				fprintf(file, "%d", image->data[i][j]);
			}
			fprintf(file, "\n");
		}
	} else if (strcmp(image->type, "P6") == 0) {
		// "Convert" P6 to P3
		fprintf(file, "P3\n%d %d\n%d\n", image->width,
				image->height, image->maxval);
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				if (j > 0) {
					fprintf(file, " ");
				}
				fprintf(file, "%d %d %d", image->data[i][j * 3],
						image->data[i][j * 3 + 1], image->data[i][j * 3 + 2]);
			}
			fprintf(file, "\n");
		}
	}
}

void handle_bin_save(FILE *file, image_t *image)
{
	// Save according to format
	if (strcmp(image->type, "P5") == 0 || strcmp(image->type, "P6") == 0) {
		fprintf(file, "%s\n%d %d\n%d\n", image->type,
				image->width, image->height, image->maxval);
		if (strcmp(image->type, "P6") == 0) {
			for (int i = 0; i < image->height; i++) {
				for (int j = 0; j < image->width; j++) {
					unsigned char r = image->data[i][j * 3];
					unsigned char g = image->data[i][j * 3 + 1];
					unsigned char b = image->data[i][j * 3 + 2];
					fwrite(&r, 1, 1, file);
					fwrite(&g, 1, 1, file);
					fwrite(&b, 1, 1, file);
				}
			}
		} else if (strcmp(image->type, "P5") == 0) {
			for (int i = 0; i < image->height; i++) {
				for (int j = 0; j < image->width; j++) {
					unsigned char pixel = (unsigned char)image->data[i][j];
					fwrite(&pixel, 1, 1, file);
				}
			}
		}
	} else if (strcmp(image->type, "P2") == 0) {
		// "Convert" P2 to P5
		fprintf(file, "P5\n%d %d\n%d\n", image->width,
				image->height, image->maxval);
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				unsigned char pixel = (unsigned char)image->data[i][j];
				fwrite(&pixel, 1, 1, file);
			}
		}
	} else if (strcmp(image->type, "P3") == 0) {
		// "Convert" P3 to P6
		fprintf(file, "P6\n%d %d\n%d\n", image->width,
				image->height, image->maxval);
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				for (int c = 0; c < 3; c++) {
					unsigned char pixel = image->data[i][j * 3 + c];
					fwrite(&pixel, 1, 1, file);
				}
			}
		}
	}
}

// Implement save_file logic
void save_file(const char *filename, image_t *image, const char *format)
{
	if (!image || !image->data) {
		printf("No image loaded\n");
		return;
	}

	FILE *file = fopen(filename, "wb");
	if (!file) {
		printf("Error opening file: %s\n", filename);
		return;
	}

	if (strcmp(format, "ascii") == 0) {
		handle_ascii_save(file, image);
	} else if (strcmp(format, "binary") == 0) {
		handle_bin_save(file, image);
	} else {
		printf("Unsupported format: %s\n", format);
	}

	fclose(file);
	printf("Saved %s\n", filename);
}
