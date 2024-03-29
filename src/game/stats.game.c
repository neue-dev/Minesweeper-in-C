/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 17:01:04
 * @ Modified time: 2024-03-30 03:20:28
 * @ Description:
 * 
 * Displays the statistics of a profile.
 */

#ifndef STATS_
#define STATS_

#include "profile.game.c"

#include "../utils/utils.string.h"

#include <stdio.h>

#define GAME_DATA_NUM 5

typedef enum GameData GameData;

enum GameData {
	GAME_TYPE,
	GAME_DIFFICULTY,
	GAME_OUTCOME,
	GAME_FIELD_WIDTH,
	GAME_FIELD_HEIGHT
};

/**
 * Saves the data of a game that had recently ended.
 * 
 * @param   { Game * }      pGame       The game object.
*/
void Stats_saveGame(Game *pGame) {
	int i, j;
	int nClassicEasy, nClassicDifficult, nCustom;   // game wins
	int dFieldSize; // number of characters in a game's field
	char *sCurrentProfile = String_alloc(PROFILE_USERNAME_MAX_LENGTH);
	char *sProfileFilePath = String_alloc(PROFILE_FILE_PATH_MAX_LENGTH);

	// Stores the game data of the last two recent games
	int dGameData[2][GAME_DATA_NUM] = {{}};

	// Stores the field of the last two recent games
	char cGameField[2][2 * (FIELD_MAX_ROWS + FIELD_MAX_COLUMNS)] = {{}};

	// Opens the profile list
	FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

	// Gets the name of the current profile
	fscanf(pProfiles, "%s", sCurrentProfile);

	fclose(pProfiles);

	// Completes the path of the profile's text file
	snprintf(sProfileFilePath, PROFILE_FILE_PATH_MAX_SIZE, "%s%s.txt",
						PROFILE_FOLDER_PATH, sCurrentProfile);

	// Opens the profile's text file
	FILE *pProfile = fopen(sProfileFilePath, "r");

	// Stores the user's previous win stats
	fscanf(pProfile, "%d %d %d ", nClassicEasy, nClassicDifficult, nCustom);

	// Stores the data of the last two most recent games
	for(i = 0; i < 2; i++) {

			// Stores the game data
			for(j = 0; j < GAME_DATA_NUM; j++) {
					fscanf(pProfile, "%d ", dGameData[i][j]);
			}
			
			// The number of characters in the game's field (considering space and newlines)
			dFieldSize = 2 * (dGameData[i][GAME_FIELD_WIDTH] + dGameData[i][GAME_FIELD_HEIGHT]);

			// Stores the game field
			for(j = 0; j < dFieldSize; j++) {
					fscanf(pProfile, "%c", cGameField[i][j]);
			}
	}

	fclose(pProfile);

	// We will now update the profile's text file
	pProfile = fopen(sProfileFilePath, "w");

	// Updates the profile's win stats if they won the most recent game
	if(pGame->eOutcome == GAME_OUTCOME_WIN) {
			nClassicEasy += (pGame->eDifficulty == GAME_DIFFICULTY_EASY);
			nClassicDifficult += (pGame->eDifficulty == GAME_DIFFICULTY_DIFFICULT);
			nCustom += (pGame->eType == GAME_TYPE_CUSTOM);
	}

	// Prints out the profile's win stats
	fprintf("%d %d %d\n", nClassicEasy, nClassicDifficult, nCustom);

	// Prints out the most recent game data
	fprintf("%d %d %d %d %d\n", pGame->eType, pGame->eDifficulty, pGame->eOutcome,
															pGame->field.dWidth, pGame->field.dHeight);

	// Prints out the most recent game's field
	for(i = 0; i < pGame->field.dHeight; i++) {
			for(j = 0; j < pGame->field.dWidth; j++) {
					
					// Prints out the flags
					// Outcome: quit/loss
					if(pGame->eOutcome != GAME_OUTCOME_WIN &&
							Grid_getBit(pGame->field.pFlagGrid, j, i))
							fprintf(pProfile, "F");
					
					// Prints out the mines
					else if(pGame->field.aNumbers[i][j] < 0) {

							// Outcome: quit; Does not show any mine
							if(pGame->eOutcome == GAME_OUTCOME_QUIT)
									fprintf(pProfile, "?");

							// Outcome: loss; Shows the mine that exploded
							else if(pGame->field.aNumbers[i][j] == -2)
									fprintf(pProfile, "H");

							// Outcome: win/loss; Shows all unexploded mines
							else
									fprintf(pProfile, "X");

					// Prints out the tile numbers
					} else {

							// Outcome: win; Shows all numbers
							// Outcome: quit/loss; Shows numbers that were revealed
							if(pGame->eOutcome == GAME_OUTCOME_WIN ||
									Grid_getBit(pGame->field.pInspectGrid, j, i))
									fprintf(pProfile, "%d", pGame->field.aNumbers[i][j]);
							
							// Outcome: quit/loss; Does not show numbers that were unrevealed
							else
									fprintf(pProfile, "?");
					}

					// Prints out a newline after every row, and space otherwise
					fprintf(pGame, (j == pGame->field.dWidth - 1) ? "\n" : " ");
			}
	}

	// Prints out the data of the last two most recent games
	for(i = 0; i < 2; i++) {

			// Prints out the game data
			for(j = 0; j < GAME_DATA_NUM; j++) {
					fprintf(pProfile, "%d", dGameData[i][j]);
					fprintf(pProfile, (j == GAME_DATA_NUM - 1 ? "\n" : " "));
			}

			// The number of characters in the game's field (considering space and newlines)
			dFieldSize = 2 * (dGameData[i][GAME_FIELD_WIDTH] + dGameData[i][GAME_FIELD_HEIGHT]);

			// Prints out the game field
			for(j = 0; j < dFieldSize; j++) {
					fprintf(pProfile, "%c", cGameField[i][j]);
			}
	}

	// Deallocates the memory of the currently-selected profile name
	String_kill(sCurrentProfile);

	// Deallocates the memory of the profile's file path
	String_kill(sProfileFilePath);

	fclose(pProfile);
}

#endif