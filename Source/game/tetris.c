#include "tetris.h"
#include "pieces.h"
#include "../GLCD/GLCD.h"
#include "graphics.h"
#include <stdbool.h>

volatile int score;
volatile int highScore = 0;
volatile int linesCleared;
volatile gameState_t currentState;
uint8_t board[ROWS][COLS];

int currentPieceID;
int currentRotation;
int currentX;
int currentY;

volatile int slowMotionTimer;


/**
 * @brief Initializes or resets the game.
 * * This function resets the score, clears the internal board matrix and prepares the first tetromino
 * * @return void
*/
void tetris_init (void) {
    score = 0;
    linesCleared = 0;
		currentPieceID = 0;
		currentRotation = 0;
		currentX = 0;
		currentY = 0;
		currentState = STATE_IDLE;

		LCD_Clear(Black);
		drawSideBar();
		drawPlayFieldBorder();
		clearMatrix();
}


/**
 * @brief Checks if a piece can move to the target position.
 * @param x: Target grid X
 * @param y: Target grid Y
 * @param rot: Target rotation
 * @param pieceID: ID of the shape
 * @return true if collision detected, false otherwise.
 */
bool checkCollision (int x, int y, int rot, int pieceID) {
	if (x >= COLS || y >= ROWS || rot >= NUM_ROTATIONS || pieceID >= NUM_TETROMINOES) // validate inputs
		return true;
		
		int r;
    for (r = 0; r < SHAPE_SIZE; r++) {
		int c;
        for (c = 0; c < SHAPE_SIZE; c++) {        
            if (TETROMINOES[pieceID][rot][r][c] != 0) { // Check if the shape actually has a block at this 4x4 coordinate
                int absoluteX = x + c;
                int absoluteY = y + r;

                if (absoluteX < 0 || absoluteX >= COLS)
                    return true; // hit wall

                if (absoluteY >= ROWS)
                    return true; // hit floor

                if (absoluteY >= 0) {
                    if (board[absoluteY][absoluteX] != 0)
                        return true; // hit another block
                }
            }
        }
    }
    return false; // safe move
}


/**
 * @brief Spawns a new random tetromino at the top of the playing field.
*/
void spawnNewPiece (void) {
    currentPieceID = rand() % NUM_TETROMINOES; // generate a random number between 0 and NUM_TETROMINOES-1
    currentRotation = rand() % NUM_ROTATIONS;
    
    currentX = (COLS / 2) - (SHAPE_SIZE / 2); 
    currentY = 0;
}


/**
 * @brief Locks the current active piece into the board matrix.
*/
void lockPiece (void) {
		int r;
    for (r = 0; r < SHAPE_SIZE; r++) {
		int c;
        for (c = 0; c < SHAPE_SIZE; c++) {
            if (TETROMINOES[currentPieceID][currentRotation][r][c] != 0) {
                int absX = currentX + c;
                int absY = currentY + r;
                
                if (absY >= 0 && absY < ROWS && absX >= 0 && absX < COLS)
                    board[absY][absX] = currentPieceID + 1;
            }
        }
    }
}


/**
 * @brief Resets the game board matrix by setting all cells to zero.
 * This acts as a helper to clear the field during initialization or restart.
*/
void clearMatrix (void) {
	int r;
	for (r = 0; r < ROWS; r++) {
		int c;
		for (c = 0; c < COLS; c++)
			board[r][c] = 0;
	}
}


/**
 * @brief Calculate the score based on the number of lines to clear.
 * Rule: Groups of 4 = 600pts. Less than 4 = 100pts * number of cleared lines.
 */
int calculateScoreForLines (int lines) {
    int total = 0;
    while (lines > 0) {
        if (lines >= 4) {
            total += 600; // tetris bonus
            lines -= 4;
        }
		else { // add 100 points for each line cleared
            total += (100 * lines);
            lines = 0;
        }
    }
    return total;
}


/**
 * @brief Replaces a random existing block with a powerup.
 */
void spawnPowerup (void) {
	struct {
		uint8_t r;
		uint8_t c;
	} candidates[ROWS * COLS];
    int r, c;
    int blockCount = 0;
    
    // count available non-empty blocks without considering existing powerups
    for (r = 0; r < ROWS; r++) {
		for (c = 0; c < COLS; c++) {
			if (board[r][c] != 0 && board[r][c] < 10){
				candidates[blockCount].r = r;
				candidates[blockCount].c = c;
				blockCount++;
			}
		}
    }
    
    if (blockCount == 0) // no blocks to turn into powerups
		return; 
    
    // select random index
    int idx = rand() % blockCount;
    int targetR = candidates[idx].r;
    int targetC = candidates[idx].c;
    
    uint8_t powerID = (rand() % 2) == 0 ? POWERUP_CLEAR_HALF : POWERUP_SLOW_DOWN;
    board[targetR][targetC] = powerID;

    // redraw the replaced block
    uint16_t x_pos = FIELD_X_START + (targetC * BLOCK_SIZE);
    uint16_t y_pos = FIELD_Y_START + (targetR * BLOCK_SIZE);
    uint16_t color = (powerID == POWERUP_CLEAR_HALF) ? White : LightGrey;
    
    LCD_FillRect(x_pos, y_pos, BLOCK_SIZE, BLOCK_SIZE, color);
}


/**
 * @brief Clear half logic
 */
void activateClearHalf(void) {
	int linesToClear = ROWS / 2;
	int startRow = ROWS - 1;
	int endRow = ROWS - linesToClear;
	
	int effectiveLines = 0;	// count how many lines in the bottom half actually have blocks
	int r;
	
	for (r = startRow; r >= endRow; r--) {
		int c;
		bool hasBlock = false;
		for (c = 0; c < COLS; c++) {
			if (board[r][c] != 0) {
				hasBlock = true;
				break; // found a block, therefore this line counts
			}
		}
		
		if (hasBlock)
			effectiveLines++;
	}
	
	if (effectiveLines == 0)
		return;
	
	// calculate score for the cleared lines (assuming 10 lines cleared)
	score += calculateScoreForLines(linesToClear);
	
	// shift the top half down by 10 rows
	for (r = endRow - 1; r >= 0; r--) {
		int c;
		for (c = 0; c < COLS; c++) {
			board[r + linesToClear][c] = board[r][c];
			board[r][c] = 0;
		}
	}
	
	refreshPlayingField(0, ROWS - 1);
}


/**
 * @brief Logic for random malus: shifts all lines up by 1 and inserts a random line at the bottom.
 */
void insertMalusLine (void) {
	int r, c;

	// if the top row is occupied, shifting up implies overflowing the board
	for (c = 0; c < COLS; c++) {
		if (board[0][c] != 0) {
			currentState = STATE_GAMEOVER;
			return;
		}
	}

	// shift all rows up
	for (r = 0; r < ROWS - 1; r++) {
		for (c = 0; c < COLS; c++)
			board[r][c] = board[r+1][c];
	}

	// generate the malus line at the bottom
	for (c = 0; c < COLS; c++)
		board[ROWS-1][c] = 0;

	int positions[COLS];
	for (c = 0; c < COLS; c++)
		positions[c] = c;

	// shuffle the positions
	for (c = COLS - 1; c > 0; c--) {
		int j = rand() % (c + 1);
		int temp = positions[c];
		positions[c] = positions[j];
		positions[j] = temp;
	}

	// fill the first 7 positions with a random colored block
	for (c = 0; c < 7; c++) {
		uint8_t randomColor = (rand() % NUM_TETROMINOES) + 1;
		board[ROWS-1][positions[c]] = randomColor;
	}

	refreshPlayingField(0, ROWS - 1);
}


/**
 * @brief Scans for full lines and removes them.
 * @return The number of lines cleared.
*/
int removeFullLines(void) {
    int linesRemovedThisTurn = 0;
    int powerupToActivate = 0;
    
    int r;
    // scan for full lines
    for (r = ROWS - 1; r >= 0; r--) {
			bool fullLine = true;
			bool emptyLine = true;
			
			int c;
			for (c = 0; c < COLS; c++) {
				if (board[r][c] == 0)
					fullLine = false;
				else
					emptyLine = false;
			}
			
			if (emptyLine) 
						break;
			
			if (fullLine) {
				// check if this line contains a powerup
				for (c = 0; c < COLS; c++) {
					if (board[r][c] == POWERUP_CLEAR_HALF)
						powerupToActivate = POWERUP_CLEAR_HALF;
					
					if (board[r][c] == POWERUP_SLOW_DOWN)
						powerupToActivate = POWERUP_SLOW_DOWN;
				}
				
				if (powerupToActivate == POWERUP_CLEAR_HALF)
					break; 
				
				linesRemovedThisTurn++;
			}
    }

    if (powerupToActivate == POWERUP_CLEAR_HALF) { // clear half overcomes normal line removal for this turn
			activateClearHalf();
			return 0; // return 0 because score was already added inside activateClearHalf()
    }
    
    if (powerupToActivate == POWERUP_SLOW_DOWN) // handle slow down
			slowMotionTimer = 15; // 15 seconds

    if (linesRemovedThisTurn > 0) {
			int writeRow = ROWS - 1;
			int readRow;
			for (readRow = ROWS - 1; readRow >= 0; readRow--) {
				bool fullLine = true;
				bool emptyLine = true;
				int x;
				for (x = 0; x < COLS; x++) {
					if (board[readRow][x] == 0)
						fullLine = false;
					else
						emptyLine = false;
				}
				
				if (emptyLine) 
					break;
				
				if (!fullLine) {
					if (writeRow != readRow) {
						for (x = 0; x < COLS; x++)
							board[writeRow][x] = board[readRow][x];
					}
					writeRow--;
				}
			}
			// clear remaining top rows
			while (writeRow >= 0) {
				int x;
				for (x = 0; x < COLS; x++)
					board[writeRow][x] = 0;
				writeRow--;
			}
			
			refreshPlayingField(0, ROWS-1);
			
			// update global counters
			int previousTotal = linesCleared;
			linesCleared += linesRemovedThisTurn;
			
			// check if we crossed a multiple of 5
			if ((linesCleared / 5) > (previousTotal / 5))
					spawnPowerup();
			
			// if we crossed a multiple of 10, trigger malus
			if ((linesCleared / 10) > (previousTotal / 10))
					insertMalusLine();
    }
    
    return linesRemovedThisTurn;
}