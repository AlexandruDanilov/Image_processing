#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "image_editor_functions.h"

// Ensures specified order of coordinates is respected
void normalize_coordinates(int *x1, int *y1, int *x2, int *y2)
{
	if (*x1 > *x2) {
		int aux = *x1;
		*x1 = *x2;
		*x2 = aux;
	}
	if (*y1 > *y2) {
		int aux = *y1;
		*y1 = *y2;
		*y2 = aux;
	}
}

void select_with_coordinates(image_t *image, selection_t *selection,
							 int x1, int y1, int x2, int y2)
{
	if (!image || !image->data) {
		printf("No image loaded\n");
		return;
	}

	// Normalize (x1 < x2, y1 < y2)
	normalize_coordinates(&x1, &y1, &x2, &y2);

	// Validate coordinates
	if (x1 < 0 || x2 > image->width || y1 < 0 ||
		y2 > image->height || x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}

	// Update selection and mark selection as active
	selection->x1 = x1;
	selection->y1 = y1;
	selection->x2 = x2;
	selection->y2 = y2;
	selection->is_selected = 1;

	// Print selection
	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

// SELECT ALL
void select_all(image_t *image, selection_t *selection)
{
	if (!image || !image->data) {
		printf("No image loaded\n");
		return;
	}
	printf("Selected ALL\n");

	// Updates selection for top left and bottom right, mark selection as active
	selection->x1 = 0;
	selection->y1 = 0;
	selection->x2 = image->width;
	selection->y2 = image->height;
	selection->is_selected = 1;

}
