#ifndef PIECES_H
#define PIECES_H

#include <stdint.h>

#define NUM_TETROMINOES 7  // total number of shape types
#define NUM_ROTATIONS   4  // total number of rotations for each shape type
#define SHAPE_SIZE      4  // grid size for a single shape

extern const uint8_t TETROMINOES[NUM_TETROMINOES][NUM_ROTATIONS][SHAPE_SIZE][SHAPE_SIZE];
	
#endif