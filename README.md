# Homework 3 PCLP - Danilov Alexandru-Cristian, 315CA

## Image Editor Problem

### Description, Input, and Output Data

The program takes a set of commands and parameters. Based on the input, the user can `LOAD`
an image in the PGM or PPM format, `SELECT` the working area, `CROP`, `ROTATE`, `EQUALIZE`,
`APPLY` certain kernels, generate the `HISTOGRAM`, and `SAVE` the image resulting from 
running the program.

- **Input**: The program expects a set of instructions on separate lines, but for it to
work, it also needs at least one `NetPBM` image saved on the computer. The program only 
works on *P2*, *P3*, *P5*, and *P6* images (*P1* and *P4* are not supported).
- **Output**: The program outputs a message according to the input command, letting the user 
know if the input is correct, the operation is supported, has been completed, or failed. 
After running the `SAVE` command, a *NetPBM* image will also be saved on the computer.

### Functionality

The purpose of the program is to process images by first selecting the working region and 
then transforming the image. For this, more subtasks had to be solved:

- **Parsing commands**: In the main function, commands are read and parsed. It is important 
that the user input is well interpreted and there are no bugs when the wrong input is given.
- **Loading and saving images**: In order for the program to function, it is important that 
*NetPBM* files are correctly loaded. Both the header and the image data have to be correctly 
stored and saved. Opened files are closed at program `EXIT`.
- **Editing images**: For the saved image to be correct, the image data must be edited 
according to the user commands.
- **Memory Allocation**: The program dynamically allocates matrices for variable-sized images 
inside structures, both for keeping the image loaded in the program memory and for temporarily 
stored data such as intermediary steps in solving the problem. Data is deallocated when no 
longer needed and also at program `EXIT`. 

### Program Interaction

The user interacts with the program by providing commands that will trigger different 
functions. Each command performs a specific task related to image processing.

---

### Implementation

### `image_editor.c`

Here, the commands are being read inside the main loop and processed. Depending on the 
command, specific parameters are being read, and corresponding functions are being called. 
The main functions for parsing the commands are *fgets* for reading the input lines, 
*strtok* to separate arguments, *strcmp* to find what command to use, and others such as 
*atoi/strtol* to convert the input text into numbers. The `free_data` function called at 
the end ensures previously allocated data is freed so no memory leaks occur. 

---

### `load_image.c` and `save_image.c`

These ensure the correct loading and saving of the image, with each image format treated 
separately. For loading, the header of the file has to be correctly interpreted, and for 
saving, it is important to make sure the header and format are right for either the *ASCII 
save* or *binary save*. The image matrix uses the `image_t` structure. 

---

### `select.c`, `crop.c`, and `rotate.c` 

After the selection is made and validated, it is stored inside `selection_t`. The user can opt to 
`CROP` the selected area, making the selection the new image, or `ROTATE` the selection if the angle 
and selection allow it. The selection has to be normalized and validated to ensure it does not extend 
out of the image, causing segfaults. For rotating selections, 90 and 270-degree angles must have a 
square selection, while 180 degrees allows rectangular rotations as well, but they have to be treated 
separately. For full image rotation, they are treated as 90-degree rotations, repeated if needed.

---

### `apply.c`

The `apply` function processes color images (`P3` or `P6`) using convolution kernels for filters. 
It modifies selected regions while keeping the unselected areas unmodified. Filters use a 3x3 kernel 
that combines neighboring pixel values with a sum, scaled by a factor. The function handles edge 
pixels by keeping their original values if neighbors are unavailable. Clamping ensures pixel values 
remain between 0 and 255.

---

### `histogram.c`

The `calculate_histogram` function generates a histogram for grayscale images (`P2` or `P5`). 
Pixel values are divided into bins, and their frequencies are visualized using stars (`*`). The 
histogram is scaled to a maximum number of stars (`max_stars`) and is proportional to pixel intensity 
distributions. It calculates bin intervals, builds the histogram, finds the maximum frequency, 
and displays the scaled result. 

---

### `equalize.c`

The `equalize_image` function improves the contrast of grayscale images using the histogram. The 
function builds the histogram and applies a transformation function to all pixel values. It clamps 
pixel values between 0 and 255. Only grayscale images (`P2` or `P5`) are supported.

---

### Program Structure

The program is modularized into multiple C files and header files for better organization, ease of 
understanding, and easier debugging:

- **Headers**: The program uses header files (`image_editor_functions.h`) to declare functions and 
define constants used throughout the code.
- **Source Files**: The program's functionality is split into several source files for better 
organization (**image_editor.c, load_file.c, save.c, select.c, crop.c, rotate.c apply.c,** 
**histogram.c, equalize.c**) and easier maintenance.

### Debugging Tools Used

- **Valgrind**: The program has been debugged with Valgrind to check for memory leaks. This ensures 
that dynamically allocated memory is properly freed and no memory is left unused.

---

### User Guide

**Build the Program**: The program is compiled using the `make` build rule. To build the program, 
run `make build`, and to clean the executables, use `make clean`.

**Run the Program**: The program is used by running `./image_editor`. It waits for instructions; 
at the end, type `EXIT` to terminate.
