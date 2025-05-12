#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_editor_functions.h"

void skip_comments(FILE *file)
{
	char line[256];
	while (fgets(line, sizeof(line), file)) {
		if (line[0] != '#') {
			// Rewind to the beginning of the line
			fseek(file, -strlen(line), SEEK_CUR);
			break;
		}
	}
}

int handle_alloc_color(image_t *image)
{
	// Allocate memory for the image on 3 channels (RGB)
	image->data = malloc(image->height * sizeof(int *));
	for (int i = 0; i < image->height; i++) {
		// 3 channels (RGB)
		image->data[i] = malloc(3 * image->width * sizeof(int));
		if (!image->data[i]) { // Handle alloc fail
			for (int j = 0; j < i; j++) {
				free(image->data[j]);
			}
			free(image->data);
			return 0; // Alloc failed
		}
	}
	return 1; // Alloc successful
}

int handle_alloc_greyscale(image_t *image)
{
	// Allocate memory for the image greyscale on 1 channel
	image->data = malloc(image->height * sizeof(int *));
	for (int i = 0; i < image->height; i++) {
		// 1 channel (grayscale)
		image->data[i] = malloc(image->width * sizeof(int));
		if (!image->data[i]) { //Handle alloc fail
			for (int j = 0; j < i; j++) {
				free(image->data[j]);
			}
			free(image->data);
			return 0; // Alloc failed
		}
	}

	return 1; // Alloc successful
}

// Load p2 images
void process_p2(FILE *file, image_t *image)
{
	int value;
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			fscanf(file, "%d", &value);
			image->data[y][x] = value;
		}
	}
}

// Load p3 images
void process_p3(FILE *file, image_t *image)
{
	int value;
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			fscanf(file, "%d", &value);
			image->data[y][3 * x] = value;
			fscanf(file, "%d", &value);
			image->data[y][3 * x + 1] = value;
			fscanf(file, "%d", &value);
			image->data[y][3 * x + 2] = value;
		}
	}
}

// Load p5 images
void process_p5(FILE *file, image_t *image)
{
	unsigned char *data = malloc(image->width * image->height);
	if (!data) { // Handle alloc fail
		for (int i = 0; i < image->height; i++) {
			free(image->data[i]);
		}
		free(image->data);
		fclose(file);
		image->loaded = 0; // Reset image status
		image->data = NULL;
		return;
	}

	fread(data, 1, image->width * image->height, file);

	// move data from aux to primary matrix
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			int value = data[y * image->width + x];
			image->data[y][x] = value;
		}
	}

	free(data); // Free the aux data
}

// Load p6 images
void process_p6(FILE *file, image_t *image)
{
	unsigned char *data = malloc(image->width * image->height * 3);
	if (!data) { // Handle alloc fail
		for (int i = 0; i < image->height; i++) {
			free(image->data[i]);
		}
		free(image->data);
		fclose(file);
		image->loaded = 0; // Reset image status
		image->data = NULL;
		return;
	}

	fread(data, 3, image->width * image->height, file);

	// Move data from aux to primary matrix
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++) {
			image->data[i][3 * j] = data[(i * image->width + j) * 3];
			image->data[i][3 * j + 1] = data[(i * image->width + j) * 3 + 1];
			image->data[i][3 * j + 2] = data[(i * image->width + j) * 3 + 2];
		}
	}

	free(data); // Free the temporary data buffer
}

// Load file, alloc and populate image
void load_file(const char *filename, image_t *image)
{
	FILE *file = fopen(filename, "rb");
	if (!file) {
		printf("Failed to load %s\n", filename);
		// Reset image status
		image->loaded = 0;
		image->data = NULL;
		return;
	}

	image->loaded = 1; // File has been loaded

	// Read header
	fread(image->type, 1, 2, file);
	image->type[2] = '\0';

	// Ensure there are no comments
	skip_comments(file);

	// Read size, skip comments
	fscanf(file, "%d %d", &image->width, &image->height);
	skip_comments(file);

	// Read maxval
	fscanf(file, "%d", &image->maxval);
	fgetc(file); // To ignore the newline after the maxval

	// Alloc memory for corresponding image type, handle errors
	if (strcmp(image->type, "P3") == 0 || strcmp(image->type, "P6") == 0) {
		if (!handle_alloc_color(image)) {
			fclose(file);
			image->loaded = 0; // Reset image state
			image->data = NULL;
			return;
		}
	} else if (strcmp(image->type, "P2") == 0 ||
			   strcmp(image->type, "P5") == 0) {
		if (!handle_alloc_greyscale(image)) {
			fclose(file);
			image->loaded = 0; // Reset image state
			image->data = NULL;
			return;
		}
	}

	// Process according to format
	if (strcmp(image->type, "P3") == 0) {
		process_p3(file, image);
	} else if (strcmp(image->type, "P6") == 0) {
		process_p6(file, image);
	} else if (strcmp(image->type, "P2") == 0) {
		process_p2(file, image);
	} else if (strcmp(image->type, "P5") == 0) {
		process_p5(file, image);
	} else {
		printf("Unknown format: %s\n", image->type);
	}
	fclose(file); // Close file
	printf("Loaded %s\n", filename);
}
