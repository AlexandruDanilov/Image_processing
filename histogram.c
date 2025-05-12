#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "image_editor_functions.h"

void calculate_histogram(image_t *image, int max_stars, int bins)
{

	if (strcmp(image->type, "P2") != 0 && strcmp(image->type, "P5") != 0) {
		printf("Black and white image needed\n");
		return;
	}

	// Calculate bin interval
	int bin_width = (image->maxval + 1) / bins;
	if (bin_width == 0)
		bin_width = 1;

	// Alloc memory for histogram
	int *histogram = calloc(bins, sizeof(int));

	// Build histogram
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++) {
			int pixel_value = image->data[i][j];
			int bin_index = pixel_value / bin_width;
			if (bin_index >= bins)
				bin_index = bins - 1;
			histogram[bin_index]++;
		}
	}

	// Find max frequency
	int max_frequency = 0;
	for (int i = 0; i < bins; i++) {
		if (histogram[i] > max_frequency) {
			max_frequency = histogram[i];
		}
	}

	// Print histogram
	for (int i = 0; i < bins; i++) {
		// Find the number of stars for each bin
		int stars = (histogram[i] * max_stars) / max_frequency;

		// Print line
		printf("%d\t|\t", stars);
		for (int j = 0; j < stars; j++) {
			printf("*");
		}
		printf("\n");
	}

	// Free alloced memory
	free(histogram);
}
