#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_editor_functions.h"

void crop(image_t *image, selection_t *selection)
{
	if (!image || !image->data) {
		printf("No image loaded\n");
		return;
	}

	if (!selection || !selection->is_selected) {
		printf("No selection made\n");
		return;
	}

	// Check selection is valid
	if (selection->x1 >= selection->x2 || selection->y1 >= selection->y2) {
		printf("Invalid selection coordinates\n");
		return;
	}

	int new_width = selection->x2 - selection->x1;
	int new_height = selection->y2 - selection->y1;

	// For color, channel count is 3 (RGB), for greyscale, channel count is 1
	int channel_count = (strcmp(image->type, "P3") == 0 ||
						 strcmp(image->type, "P6") == 0) ? 3 : 1;

	// Alloc memory for cropped image data
	int **new_data = malloc(new_height * sizeof(int *));

	for (int i = 0; i < new_height; i++) {
		new_data[i] = malloc(new_width * channel_count * sizeof(int));
	}

	// Copy selected data in new image
	for (int i = 0; i < new_height; i++) {
		for (int j = 0; j < new_width; j++) {
			for (int c = 0; c < channel_count; c++) {
				new_data[i][j * channel_count + c] =
					image->data[selection->y1 + i][(selection->x1 + j)
					* channel_count + c];
			}
		}
	}

	// Free old image data
	for (int i = 0; i < image->height; i++) {
		free(image->data[i]);
	}
	free(image->data);

	// Update image
	image->data = new_data;
	image->width = new_width;
	image->height = new_height;

	// Update selection
	selection->x1 = 0;
	selection->y1 = 0;
	selection->x2 = new_width;
	selection->y2 = new_height;
	selection->is_selected = 1;

	printf("Image cropped\n");
}
