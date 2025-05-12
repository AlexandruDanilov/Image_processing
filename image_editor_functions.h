#pragma once

// Structure for image
typedef struct {
	int **data; // Matrix for image data
	int width;
	int height;
	int maxval;
	char type[3];
	int loaded; // 1 if loaded, 0 if not
} image_t;

// Structure for selection data
typedef struct {
	int x1, y1, x2, y2;
	int is_selected; // 1 if loaded 0 if not
} selection_t;

// Function declarations
void load_file(const char *filename, image_t *image);

void select_all(image_t *image, selection_t *selection);

void select_with_coordinates(image_t *image, selection_t *selection,
							 int x1, int y1, int x2, int y2);

void rotate(image_t *image, selection_t *selection, int angle);

void crop(image_t *image, selection_t *selection);

void apply(const char *param, image_t *image, selection_t *selection);

void save_file(const char *filename, image_t *image, const char *format);

void calculate_histogram(image_t *image, int bins, int max_stars);

void equalize_image(image_t *image);
