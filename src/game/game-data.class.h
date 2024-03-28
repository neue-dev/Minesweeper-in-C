/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-09 19:56:58
 * @ Modified time: 2024-03-28 19:59:43
 * @ Description:
 * 
 * Represents account objects we have during runtime
 */

#ifndef GAME_DATA_
#define GAME_DATA_

#include <stdio.h>
#include <stdlib.h>

typedef struct GameData GameData;

/**
 * A class consisting of the data of a game.
 * The data associated with the game will be read from a text file.
 * 
 * @class
*/
struct GameData {
    int eType;          // Game type (classic/custom)
    int eDifficulty;    // Game difficulty (easy/difficult)
    int eOutcome;       // Game outcome (quit/loss/win)

    int dFieldWidth;    // Width of the game's minefield
    int dFieldHeight;   // Height of the game's minefield
};
/**
 * Allocates memory for an instance of the GameData class.
 * 
 * @return	{ GameData * }		A pointer to the allocated memory.
*/
GameData *GameData_new() {
  GameData *pGameData = calloc(1, sizeof(GameData));
  return pGameData;
}

/**
 * Initializes an instance of the GameData class.
 * 
 * @param		{ GameData * }		this	A pointer to the instance to initialize.
 * @return	    { GameData * }				A pointer to the initialized instance.
*/
GameData *GameData_init(GameData *this) {
  return this;
}

/**
 * Creates an initialized instance of the GameData class.
 * 
 * @return	{ GameData * }		A pointer to the newly created initialized instance.
*/
GameData *GameData_create() {
  return GameData_init(GameData_new());
}

/**
 * Deallocates the memory of an instance of the GameData class.
 * 
 * @param		{ GameData * }		this	A pointer to the instance to deallocate.
*/
void GameData_kill(GameData *this) {
  free(this);
}

#endif