/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 10:55:29
 * @ Modified time: 2024-03-28 11:27:18
 * @ Description:
 * 
 * Holds the game struct that stores all of the game state.
 */

#ifndef GAME_
#define GAME_

#include "./field.obj.h"
#include "./gameplay.game.c"

typedef struct Game Game;

struct Game {
  Field gameField;  // Game field
  int dTime;        // Game timer
};

void Game_init(Game *this) {
  this->dTime = 0;
  
  Field_init(&this->gameField, 10, 10);
}

#endif

