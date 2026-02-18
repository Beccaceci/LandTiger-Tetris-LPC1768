#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

#define BLOCK_SIZE    16 
#define FIELD_X_START 0   
#define FIELD_Y_START 0

void drawValue(uint16_t, uint16_t, int, uint16_t);
void drawSideBar(void);
void updateScoreDisplay(void);
void drawTetromino(int, int, int, int, bool);
void refreshPlayingField(int, int);
void drawPlayFieldBorder(void);

#endif