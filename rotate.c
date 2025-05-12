#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "image_editor_functions.h"

void rotate_selection(image_t *image, selection_t *selection, int angle)
{
	int start_x = selection->x1, start_y = selection->y1;
	int end_x = selection->x2, end_y = selection->y2;
	int width = end_x - start_x;
	int height = end_y - start_y;

	// For color, channel count is 3 (RGB), for greyscale, channel count is 1
	int channel_count = (strcmp(image->type, "P3") == 0 ||
						 strcmp(image->type, "P6") == 0) ? 3 : 1;

	// Alloc memory for rotated data
	int **rotated_data = malloc(height * sizeof(int *));
	for (int i = 0; i < height; i++) {
		rotated_data[i] = malloc(width * channel_count * sizeof(int));
	}

	// Rotate according to angle:
	// Rotate 90 degrees
	if (angle == 90) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				for (int c = 0; c < channel_count; c++) {
					rotated_data[j][(height - 1 - i)
					* channel_count + c] =
						image->data[start_y + i][(start_x + j)
					* channel_count + c];
				}
			}
		}
	} else if (angle == 180) {
		// Rotate 180 degrees
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				for (int c = 0; c < channel_count; c++) {
					rotated_data[height - 1 - i][(width - 1 - j)
					* channel_count + c] =
						image->data[start_y + i][(start_x + j)
					* channel_count + c];
				}
			}
		}
	} else if (angle == 270) {
		// Rotate 270 degrees
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				for (int c = 0; c < channel_count; c++) {
					rotated_data[width - 1 - j][i
					* channel_count + c] =
						image->data[start_y + i][(start_x + j)
					* channel_count + c];
				}
			}
		}
	}

	// Copy rotated data inside image
	if (angle) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				for (int c = 0; c < channel_count; c++) {
					image->data[start_y + i][(start_x + j)
					* channel_count + c] =
						rotated_data[i][j * channel_count + c];
				}
			}
		}
	}

	// Free allocated memory
	for (int i = 0; i < height; i++) {
		free(rotated_data[i]);
	}
	free(rotated_data);
}

// Rotate image
void rotate_full_image(image_t *image, int angle, selection_t *selection)
{
	int width = image->width;
	int height = image->height;

	// For color, channel count is 3 (RGB), for grayscale, channel count is 1
	int channel_count = (strcmp(image->type, "P3") == 0 ||
						 strcmp(image->type, "P6") == 0) ? 3 : 1;

	if (!image || !image->data) {
		printf("No image loaded\n");
		return;
	}

	// Rotate 90 degrees
	if (angle == 90 || angle == -270) {
		int **rotated_data = malloc(width * sizeof(int *));
		for (int i = 0; i < width; i++) {
			rotated_data[i] = malloc(height * channel_count * sizeof(int));
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				for (int c = 0; c < channel_count; c++) {
					rotated_data[j][(height - 1 - i) * channel_count + c] =
						image->data[i][j * channel_count + c];
				}
			}
		}
		image->width = height;
		image->height = width;

		// Free allocated memory
		for (int i = 0; i < height; i++) {
			free(image->data[i]);
		}
		free(image->data);

		image->data = rotated_data;
	} else if (angle == 180 || angle == -180) {
		rotate_full_image(image, 90, selection);
		rotate_full_image(image, 90, selection);
	} else if (angle == 270 || angle == -90) {
		rotate_full_image(image, 90, selection);
		rotate_full_image(image, 90, selection);
		rotate_full_image(image, 90, selection);
	} else if (angle == 360 || angle == 0) {
		;
	} else { // Handle unsupported angle
		printf("Unsupported rotation angle\n");
		return;
	}

	// Adjust selection after rotation
	if (selection) {
		selection->x1 = 0;
		selection->y1 = 0;
		selection->x2 = image->width;
		selection->y2 = image->height;
		selection->is_selected = true;
	}
}

void rotate(image_t *image, selection_t *selection, int angle)
{
	if (!image || !image->data) {
		printf("No image loaded\n");
		return;
	}

	// Check angle
	if (abs(angle) % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}

	// If angle is supported print message
	printf("Rotated %d\n", angle);

	// Normalize
	if (angle < 0) {
		angle = 360 + (angle % 360);
	}
	angle = angle % 360;

	if (angle == 180) {
		// For 180 rotation can be applied regardless of shape
		if (!selection->is_selected) {
			rotate_full_image(image, angle, selection);
		} else {
			rotate_selection(image, selection, angle);
		}
	} else {
		// Check for square selection, rotate full image for no selection
		if (!selection->is_selected) {
			rotate_full_image(image, angle, selection);
		} else {
			int width = selection->x2 - selection->x1;
			int height = selection->y2 - selection->y1;

			// Check if selection and image are the same
			if ((selection->x1 == 0 && selection->y1 == 0 &&
				 selection->x2 == image->width &&
				 selection->y2 == image->height) || !selection->is_selected) {
				rotate_full_image(image, angle, selection);
			} else if (width == height) {
				// If selection is square rotate
				rotate_selection(image, selection, angle);
			} else {
				// Handle non-square selection
				if (angle != 0) {
					printf("The selection must be square\n");
				}
			}
		}
	}
}
