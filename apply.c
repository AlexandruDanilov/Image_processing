#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "image_editor_functions.h"

// Clamp pixels [0, 255]
int clamp(int value)
{
	if (value < 0)
		return 0;
	if (value > 255)
		return 255;
	return value;
}

// Allocate memory for the image with applied kernel
int **alloc_new(int height, int width)
{
	int **new_data = malloc(height * sizeof(int *));
	for (int i = 0; i < height; i++) {
		new_data[i] = malloc(width * 3 * sizeof(int));
	}
	return new_data;
}

// Apply kernel on pixel
void apply_pixel(image_t *image, float kernel[3][3],
				 int x, int y, int **new_data, float kernel_factor)
{
	int new_pixel[3] = {0}; // For RGB values

	// Iterate through neighbors
	for (int ky = -1; ky <= 1; ky++) {
		for (int kx = -1; kx <= 1; kx++) {
			for (int c = 0; c < 3; c++) {
				float pixel_value = image->data[y + ky][(x + kx) * 3 + c];
				new_pixel[c] += pixel_value * kernel[ky + 1][kx + 1];
			}
		}
	}

	// Multiply with factor depending on kernel
	for (int c = 0; c < 3; c++) {
		new_pixel[c] *= kernel_factor;
		// Round and clamp
		new_data[y][(x * 3) + c] = clamp((int)roundf(new_pixel[c]));
	}
}

// Apply kernel on selection
void apply_selection(image_t *image, float kernel[3][3], int **new_data,
					 selection_t *selection, float kernel_factor)
{
	int x1 = selection->x1, y1 = selection->y1;
	int x2 = selection->x2, y2 = selection->y2;
	int width = image->width;
	int height = image->height;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// Check pixel is selected
			if (x >= x1 && x < x2 && y >= y1 && y < y2) {
				// Check pixel has neighbours
				if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
					apply_pixel(image, kernel, x, y, new_data, kernel_factor);
				} else {
					// Edge pixels keep their value
					for (int c = 0; c < 3; c++) {
						new_data[y][(x * 3) + c] = image->data[y][(x * 3) + c];
					}
				}
			} else {
				// Pixels outside selection keep their value
				for (int c = 0; c < 3; c++) {
					new_data[y][(x * 3) + c] = image->data[y][(x * 3) + c];
				}
			}
		}
	}
}

// Apply kernel on image
void apply_kernel(image_t *image, float kernel[3][3], const char *filter_type,
				  selection_t *selection, float kernel_factor)
{
	if (!image || !image->data) {
		printf("No image loaded\n");
		return;
	}

	if (strcmp(image->type, "P3") != 0 && strcmp(image->type, "P6") != 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	int width = image->width;
	int height = image->height;

	int **new_data = alloc_new(height, width);
	apply_selection(image, kernel, new_data, selection, kernel_factor);

	// Update original matrix
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width * 3; j++) {
			image->data[i][j] = new_data[i][j];
		}
	}

	// Free memory for aux matrix
	for (int i = 0; i < height; i++) {
		free(new_data[i]);
	}
	free(new_data);

	printf("APPLY %s done\n", filter_type);
}

// Handle params and call apply_kernel according to kernel applied
void apply(const char *param, image_t *image, selection_t *selection)
{
	if (!image || !image->data) {
		printf("No image loaded\n");
		return;
	}

	if (!param || strlen(param) == 0) {
		printf("Invalid command\n");
		return;
	}

	if (strcmp(param, "EDGE") == 0) {
		float edge_kernel[3][3] = {
			{ -1, -1, -1 },
			{ -1,  8, -1 },
			{ -1, -1, -1 }
		};
		apply_kernel(image, edge_kernel, "EDGE", selection, 1);
	} else if (strcmp(param, "SHARPEN") == 0) {
		float sharpen_kernel[3][3] = {
			{  0, -1,  0 },
			{ -1,  5, -1 },
			{  0, -1,  0 }
		};
		apply_kernel(image, sharpen_kernel, "SHARPEN", selection, 1);
	} else if (strcmp(param, "BLUR") == 0) {
		float blur_kernel[3][3] = {
			{ 1, 1, 1 },
			{ 1, 1, 1 },
			{ 1, 1, 1 }
		};
		apply_kernel(image, blur_kernel, "BLUR", selection, 1.0f / 9.0f);
	} else if (strcmp(param, "GAUSSIAN_BLUR") == 0) {
		float gaussian_blur_kernel[3][3] = {
			{ 1, 2, 1 },
			{ 2, 4, 2 },
			{ 1, 2, 1 }
		};
		apply_kernel(image, gaussian_blur_kernel,
					 "GAUSSIAN_BLUR", selection, 1.0f / 16.0f);
	} else {
		printf("APPLY parameter invalid\n");
	}
}
