#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "image_editor_functions.h"

void equalize_image(image_t *image)
{
	int area = image->width * image->height; // Calculate total area

	// Handle ppm images
	if (strcmp(image->type, "P2") != 0 && strcmp(image->type, "P5") != 0) {
		printf("Black and white image needed\n");
		return;
	}

	// Calculate histogram
	int *histogram = calloc(image->maxval + 1, sizeof(int));
	// Allocated memory with 0 for histogram

	// Calculate pixel frequency
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			int pixel_value = image->data[y][x];
			histogram[pixel_value]++;
		}
	}

	// Allocate with 0 memory for h sum
	double *h_sum = calloc(image->maxval + 1, sizeof(double));

	h_sum[0] = histogram[0]; // Starting value for h_sum is histogram[0]
	for (int i = 1; i <= image->maxval; i++) {
		h_sum[i] = h_sum[i - 1] + histogram[i]; // Calculate sums
	}

	// Apply f(a), round and clamp to obtain the new pixel value
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++) {
			int pixel_value = image->data[i][j];

			// f(a) = round(255 * H(a) / Area)
			double transformed_value = 255.0 * (h_sum[pixel_value] / area);
			int new_pixel_value = (int)round(transformed_value);

			// Clamp
			if (new_pixel_value < 0) {
				new_pixel_value = 0;
			}

			if (new_pixel_value > 255) {
				new_pixel_value = 255;
			}

			// Assign new value to pixel
			image->data[i][j] = new_pixel_value;
		}
	}
	// free aux memory
	free(histogram);
	free(h_sum);

	printf("Equalize done\n");
}
