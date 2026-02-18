#include <stdio.h>
#include "graphics.h"
#include "../GLCD/GLCD.h"
#include "tetris.h"
#include "pieces.h"


/**
 * @brief map ID to color.
 * @param id.
 * @return color.
*/
static uint16_t getColorFromID (uint8_t id) {
    switch(id) {
				case 0: return Black;   // empty block
        case 1: return Cyan;    // I-shape
        case 2: return Yellow;  // O-shape
        case 3: return Magenta; // T-shape
        case 4: return Blue;    // J-shape
        case 5: return Orange;  // L-shape
        case 6: return Green;   // S-shape
        case 7: return Red;     // Z-shape
				case 10: return White;       // clear half
        case 11: return LightGrey;   // slow down
        default: return Black;
    }
}


/**
 * @brief Draws a numerical value at a specific coordinate.
 * @param x: X-coordinate on the LCD.
 * @param y: Y-coordinate on the LCD.
 * @param value: The integer value to display.
 * @param color: The 16-bit color for the text.
*/
void drawValue (uint16_t x, uint16_t y, int value, uint16_t color) {
    char buffer[20];
    sprintf(buffer, "%d", value); // used to convert int to string
    GUI_Text(x, y, (uint8_t *)buffer, color, Black);
}


/**
 * @brief Draws the right sidebar
*/
void drawSideBar (void) {
    GUI_Text(170, 20,  (uint8_t *)"TOP", White, Black);
    GUI_Text(170, 60,  (uint8_t *)"SCORE", White, Black);
    GUI_Text(170, 100, (uint8_t *)"LINES", White, Black);

    updateScoreDisplay();
}


/**
 * @brief Refreshes the score numbers.
*/
void updateScoreDisplay (void) {
    drawValue(170, 40, highScore, White);
    drawValue(170, 80, score, White);
    drawValue(170, 120, linesCleared, White);
}


/**
 * @brief Draws or erases a tetromino.
 * @param erase if true, draws the shape's color otherwise.
*/
void drawTetromino (int x, int y, int rot, int pieceID, bool erase) {
	uint16_t color = (erase) ? Black : getColorFromID(pieceID+1);

	int r;
    for (r = 0; r < SHAPE_SIZE; r++) {
		int c;
        for (c = 0; c < SHAPE_SIZE; c++) {
            if (TETROMINOES[pieceID][rot][r][c] != 0) {
                int gridX = x + c;
                int gridY = y + r;
                
                if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS) { //check if it is a lecit position
                    uint16_t screenX = FIELD_X_START + (gridX * BLOCK_SIZE);
                    uint16_t screenY = FIELD_Y_START + (gridY * BLOCK_SIZE);
                    
                    LCD_FillRect(screenX, screenY, BLOCK_SIZE, BLOCK_SIZE, color); //draw the block
                }
            }
        }
    }
}


/**
 * @brief Redraws only a specific range of rows of the playing field.
 * @param startRow: The starting row index
 * @param endRow: The ending row index
 */
void refreshPlayingField (int startRow, int endRow) {
    int y;
    if(startRow < 0)
		startRow = 0;
		
    if(endRow >= ROWS)
		endRow = ROWS - 1;

    for(y = endRow; y >= startRow; y--) {
        int x;
        for(x = 0; x < COLS; x++) {
            uint16_t x_pos = FIELD_X_START + (x * BLOCK_SIZE);
            uint16_t y_pos = FIELD_Y_START + (y * BLOCK_SIZE);
            
            uint8_t cellID = board[y][x];
            uint16_t colorToDraw = getColorFromID(cellID);
            LCD_FillRect(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, colorToDraw);
        }
    }
}


/**
 * @brief Draws the vertical separator line between the game and the scoreboard.
 */
void drawPlayFieldBorder (void) {
    uint16_t color = White; 

    // calculate where the grid ends
    uint16_t gridWidth = COLS * BLOCK_SIZE;
    uint16_t gridEndX  = FIELD_X_START + gridWidth;

    // calculate the separator position
    if (gridEndX < 170) {
        uint16_t separatorX = (gridEndX + 170) / 2;
        LCD_DrawLine(separatorX, 0, separatorX, 320, color); // draw a vertical line
    }
}