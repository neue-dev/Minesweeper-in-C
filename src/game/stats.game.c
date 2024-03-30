/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 17:01:04
 * @ Modified time: 2024-03-30 23:28:38
 * @ Description:
 * 
 * Displays the statistics of a profile.
 */

#ifndef STATS_
#define STATS_

#include "./game.c"
#include "./profile.game.c"

#include "../utils/utils.string.h"

#include <stdio.h>

/**
 * Saves the information held by the most recent game.
 * 
 * @param		{ Game * }	pGame		The game object.
*/
void Stats_update(Game *pGame) {
	int i, j;

	// The profile file
	char *sProfilePath;
	File *pProfileFile;

	// The general stats
	int nClassicEasyWins = 0, nClassicDifficultWins = 0, nCustomWins = 0;
	int nClassicEasyLoss = 0, nClassicDifficultLoss = 0, nCustomLoss = 0;
	char sClassicEasy[32] = { 0 }, sClassicDifficult[32] = { 0 }, sCustom[32] = { 0 };

	// The latest game data
	char sGameData[256];
	char sGameGridData[GAME_MAX_ROWS][GAME_MAX_COLUMNS];

	// The data in the profile file
	int nProfileDataLength = 0;
	int nProfileNewDataLength = 0;
	char *sProfileDataArray[PROFILE_FILE_MAX_HEIGHT + 1];
	char *sProfileNewDataArray[PROFILE_FILE_MAX_HEIGHT + 1];

	// Path to the profile
	sProfilePath = String_alloc(PROFILE_USERNAME_MAX_LENGTH + strlen(PROFILE_FOLDER_PATH) + 8);
	sprintf(sProfilePath, "%s%s.txt", PROFILE_FOLDER_PATH, pGame->pProfile->sCurrentProfile);
	
	// If file doesn't exist, exit
	if(!File_exists(sProfilePath))
		return;

	// New profile file
	pProfileFile = File_create(sProfilePath);

	// Read the file
	File_readText(pProfileFile, PROFILE_FILE_MAX_HEIGHT + 1, &nProfileDataLength, sProfileDataArray);
	nProfileNewDataLength = nProfileDataLength;

	// Read the file header
	for(i = 0; i < PROFILE_FILE_HEADER_HEIGHT; i++) {
		j = 0;

		// Skip the name
		while(sProfileDataArray[i][++j] != ':');

		// Read each win parameter
		String_clear(sClassicEasy);
		String_clear(sClassicDifficult);
		String_clear(sCustom);
		
		while(sProfileDataArray[i][++j] != ',') {
			switch(i) {
				case 0: sprintf(sClassicEasy, "%s%c", sClassicEasy, sProfileDataArray[i][j]); break;
				case 1: sprintf(sClassicDifficult, "%s%c", sClassicDifficult, sProfileDataArray[i][j]); break;
				case 2: sprintf(sCustom, "%s%c", sCustom, sProfileDataArray[i][j]); break;
				default: break;
			}
		}

		switch(i) {
			case 0: nClassicEasyWins = atoi(sClassicEasy); break;
			case 1: nClassicDifficultWins = atoi(sClassicDifficult); break;
			case 2: nCustomWins = atoi(sCustom); break;
			default: break;
		}

		// Read each loss parameter
		String_clear(sClassicEasy);
		String_clear(sClassicDifficult);
		String_clear(sCustom);
		
		while(sProfileDataArray[i][++j] != ';') {
			switch(i) {
				case 0: sprintf(sClassicEasy, "%s%c", sClassicEasy, sProfileDataArray[i][j]); break;
				case 1: sprintf(sClassicDifficult, "%s%c", sClassicDifficult, sProfileDataArray[i][j]); break;
				case 2: sprintf(sCustom, "%s%c", sCustom, sProfileDataArray[i][j]); break;
				default: break;
			}
		}

		switch(i) {
			case 0: nClassicEasyLoss = atoi(sClassicEasy); break;
			case 1: nClassicDifficultLoss = atoi(sClassicDifficult); break;
			case 2: nCustomLoss = atoi(sCustom); break;
			default: break;
		}
	}

	// Update the player stats
	sprintf(sClassicEasy, "-CLASSIC_EASY:%d,%d;\n", nClassicEasyWins + 1, nClassicEasyLoss);
	sprintf(sClassicDifficult, "-CLASSIC_DIFFICULT:%d,%d;\n", nClassicDifficultWins + 1, nClassicDifficultLoss);
	sprintf(sCustom, "-CUSTOM:%d,%d;\n", nCustomWins, nCustomLoss + 1);

	sProfileNewDataArray[0] = String_create(sClassicEasy);
	sProfileNewDataArray[1] = String_create(sClassicDifficult);
	sProfileNewDataArray[2] = String_create(sCustom);

	// Rewrite the file contents
	File_clear(pProfileFile);
	File_writeText(pProfileFile, nProfileNewDataLength, sProfileNewDataArray);

	// Write the latest game onto the buffer
	// Note that we store all the games...

	// Garbage collection
	File_freeBuffer(nProfileDataLength, sProfileDataArray);
	File_freeBuffer(nProfileNewDataLength, sProfileNewDataArray);
}

/**
 * Saves the data of a game that had recently ended.
 * 
 * @param   { Game * }      pGame       The game object.
*/
void oldStats_saveGame(Game *pGame) {
	// int i, j;
	// int nClassicEasy, nClassicDifficult, nCustom;   // game wins
	// int dFieldSize; // number of characters in a game's field
	// char *sCurrentProfile = String_alloc(PROFILE_USERNAME_MAX_LENGTH);
	// char *sProfileFilePath = String_alloc(PROFILE_FILE_PATH_MAX_LENGTH);

	// // Stores the game data of the last two recent games
	// int dGameData[2][GAME_DATA_NUM] = {{}};

	// // Stores the field of the last two recent games
	// char cGameField[2][2 * (FIELD_MAX_ROWS + FIELD_MAX_COLUMNS)] = {{}};

	// // Opens the profile list
	// FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

	// // Gets the name of the current profile
	// fscanf(pProfiles, "%s", sCurrentProfile);

	// fclose(pProfiles);

	// // Completes the path of the profile's text file
	// snprintf(sProfileFilePath, PROFILE_FILE_PATH_MAX_SIZE, "%s%s.txt",
	// 					PROFILE_FOLDER_PATH, sCurrentProfile);

	// // Opens the profile's text file
	// FILE *pProfile = fopen(sProfileFilePath, "r");

	// // Stores the user's previous win stats
	// fscanf(pProfile, "%d %d %d ", nClassicEasy, nClassicDifficult, nCustom);

	// // Stores the data of the last two most recent games
	// for(i = 0; i < 2; i++) {

	// 		// Stores the game data
	// 		for(j = 0; j < GAME_DATA_NUM; j++) {
	// 				fscanf(pProfile, "%d ", dGameData[i][j]);
	// 		}
			
	// 		// The number of characters in the game's field (considering space and newlines)
	// 		dFieldSize = 2 * (dGameData[i][GAME_FIELD_WIDTH] + dGameData[i][GAME_FIELD_HEIGHT]);

	// 		// Stores the game field
	// 		for(j = 0; j < dFieldSize; j++) {
	// 				fscanf(pProfile, "%c", cGameField[i][j]);
	// 		}
	// }

	// fclose(pProfile);

	// // We will now update the profile's text file
	// pProfile = fopen(sProfileFilePath, "w");

	// // Updates the profile's win stats if they won the most recent game
	// if(pGame->eOutcome == GAME_OUTCOME_WIN) {
	// 		nClassicEasy += (pGame->eDifficulty == GAME_DIFFICULTY_EASY);
	// 		nClassicDifficult += (pGame->eDifficulty == GAME_DIFFICULTY_DIFFICULT);
	// 		nCustom += (pGame->eType == GAME_TYPE_CUSTOM);
	// }

	// // Prints out the profile's win stats
	// fprintf("%d %d %d\n", nClassicEasy, nClassicDifficult, nCustom);

	// // Prints out the most recent game data
	// fprintf("%d %d %d %d %d\n", pGame->eType, pGame->eDifficulty, pGame->eOutcome,
	// 														pGame->field.dWidth, pGame->field.dHeight);

	// // Prints out the most recent game's field
	// for(i = 0; i < pGame->field.dHeight; i++) {
	// 		for(j = 0; j < pGame->field.dWidth; j++) {
					
	// 				// Prints out the flags
	// 				// Outcome: quit/loss
	// 				if(pGame->eOutcome != GAME_OUTCOME_WIN &&
	// 						Grid_getBit(pGame->field.pFlagGrid, j, i))
	// 						fprintf(pProfile, "F");
					
	// 				// Prints out the mines
	// 				else if(pGame->field.aNumbers[i][j] < 0) {

	// 						// Outcome: quit; Does not show any mine
	// 						if(pGame->eOutcome == GAME_OUTCOME_QUIT)
	// 								fprintf(pProfile, "?");

	// 						// Outcome: loss; Shows the mine that exploded
	// 						else if(pGame->field.aNumbers[i][j] == -2)
	// 								fprintf(pProfile, "H");

	// 						// Outcome: win/loss; Shows all unexploded mines
	// 						else
	// 								fprintf(pProfile, "X");

	// 				// Prints out the tile numbers
	// 				} else {

	// 						// Outcome: win; Shows all numbers
	// 						// Outcome: quit/loss; Shows numbers that were revealed
	// 						if(pGame->eOutcome == GAME_OUTCOME_WIN ||
	// 								Grid_getBit(pGame->field.pInspectGrid, j, i))
	// 								fprintf(pProfile, "%d", pGame->field.aNumbers[i][j]);
							
	// 						// Outcome: quit/loss; Does not show numbers that were unrevealed
	// 						else
	// 								fprintf(pProfile, "?");
	// 				}

	// 				// Prints out a newline after every row, and space otherwise
	// 				fprintf(pGame, (j == pGame->field.dWidth - 1) ? "\n" : " ");
	// 		}
	// }

	// // Prints out the data of the last two most recent games
	// for(i = 0; i < 2; i++) {

	// 		// Prints out the game data
	// 		for(j = 0; j < GAME_DATA_NUM; j++) {
	// 				fprintf(pProfile, "%d", dGameData[i][j]);
	// 				fprintf(pProfile, (j == GAME_DATA_NUM - 1 ? "\n" : " "));
	// 		}

	// 		// The number of characters in the game's field (considering space and newlines)
	// 		dFieldSize = 2 * (dGameData[i][GAME_FIELD_WIDTH] + dGameData[i][GAME_FIELD_HEIGHT]);

	// 		// Prints out the game field
	// 		for(j = 0; j < dFieldSize; j++) {
	// 				fprintf(pProfile, "%c", cGameField[i][j]);
	// 		}
	// }

	// // Deallocates the memory of the currently-selected profile name
	// String_kill(sCurrentProfile);

	// // Deallocates the memory of the profile's file path
	// String_kill(sProfileFilePath);

	// fclose(pProfile);
}

#endif