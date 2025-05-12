#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "image_editor_functions.h"

#define CMAX 100
#define PMAX 10

// Separate arguments with strtok
void process_command(char *line, char *command,
					 char params[PMAX][CMAX], int *param_count)
{
	char *token = strtok(line, " "); // Separate using spaces
	if (token) {
		strncpy(command, token, CMAX - 1); // Copy the command
		command[CMAX - 1] = '\0';
	}

	// Init param count
	*param_count = 0;

	// Read params
	while ((token = strtok(NULL, " ")) != NULL && *param_count < PMAX) {
		// Strncpy to copy each param
		strncpy(params[*param_count], token, CMAX - 1);
		params[*param_count][CMAX - 1] = '\0';
		(*param_count)++;
	}
}

// Function to free image
void free_data(image_t *image)
{
	if (image->data) {
		for (int i = 0; i < image->height; i++) {
			free(image->data[i]);
		}
		free(image->data);
	}
}

int main(void)
{
	char command[CMAX], params[PMAX][CMAX], input_line[CMAX];
	int param_count = 0, x1, y1, x2, y2, angle;
	image_t image = {0}; selection_t selection = {0};
	bool run = true;
	while (run) {
		if (!fgets(input_line, sizeof(input_line), stdin)) {
			break; // Exit if input is null or end of file
		}
		input_line[strcspn(input_line, "\n")] = '\0';
		process_command(input_line, command, params, &param_count);
		if (strcmp(command, "LOAD") == 0) {
			free_data(&image);
			if (param_count >= 1)
				load_file(params[0], &image);
			if (image.data) {
				selection.x1 = 0; selection.y1 = 0;
				selection.x2 = image.width;
				selection.y2 = image.height;
				selection.is_selected = 1; // Marks selection as active
			}
		} else if (strcmp(command, "SELECT") == 0) {
			if (param_count == 1 && strcmp(params[0], "ALL") == 0)
				select_all(&image, &selection);
			else if (param_count == 4) {
				char *endptr;
				x1 = strtol(params[0], &endptr, 10);
				y1 = strtol(params[1], &endptr, 10);
				x2 = strtol(params[2], &endptr, 10);
				y2 = strtol(params[3], &endptr, 10);
				if (*endptr != '\0')
					printf("Invalid command\n"); // Check all params are valid
				else
					select_with_coordinates(&image, &selection, x1, y1, x2, y2);
			} else
				printf("Invalid command\n");
		} else if (strcmp(command, "ROTATE") == 0) {
			if (param_count == 1) {
				angle = atoi(params[0]);
				rotate(&image, &selection, angle);
			} else
				printf("Invalid command\n");
		} else if (strcmp(command, "EQUALIZE") == 0) {
			if (!image.data) {
				printf("No image loaded\n");
			} else
				equalize_image(&image);
		} else if (strcmp(command, "CROP") == 0) {
			crop(&image, &selection);
		} else if (strcmp(command, "APPLY") == 0) {
			if (param_count >= 1)
				apply(params[0], &image, &selection);
			else
				apply(NULL, &image, &selection); // For no params call null
				// No params situation is handled in apply function
		} else if (strcmp(command, "SAVE") == 0) {
			if (param_count >= 1) {
				const char *format_str = "binary"; // binary format for save
				if (param_count == 2 && strcmp(params[1], "ascii") == 0)
					format_str = "ascii"; // ascii format for save
				save_file(params[0], &image, format_str);
			}
		} else if (strcmp(command, "HISTOGRAM") == 0) {
			if (!image.data) {
				printf("No image loaded\n");
			} else if (param_count == 2) {
				int bins = atoi(params[0]), max_stars = atoi(params[1]);
				calculate_histogram(&image, bins, max_stars);
			} else
				printf("Invalid command\n");
				// Invalid for less or more than 2 params
		} else if (strcmp(command, "EXIT") == 0) {
			if (!image.data)
				printf("No image loaded\n");
			run = 0;
		} else
			printf("Invalid command\n");
	}
	free_data(&image); // Free image before termination
	return 0;
}
