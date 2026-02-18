#ifndef TETRIS_H
#define TETRIS_H

#define ROWS 20	// number of blocks on Y-axis
#define COLS 10 // number of blocks on X-axis

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STATE_IDLE,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER
} gameState_t;

#define POWERUP_CLEAR_HALF 10
#define POWERUP_SLOW_DOWN  11

extern volatile int score;
extern volatile int highScore;
extern volatile int linesCleared;
extern volatile gameState_t currentState;
extern uint8_t board[ROWS][COLS];

extern int currentPieceID;
extern int currentRotation;
extern int currentX;
extern int currentY;

extern volatile int slowMotionTimer;

void tetris_init(void);
bool checkCollision(int, int, int, int);
void spawnNewPiece(void);
void lockPiece(void);
void clearMatrix(void);
int calculateScoreForLines(int);
void spawnPowerup(void);
int removeFullLines(void);

#endif