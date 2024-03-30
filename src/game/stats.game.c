/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 17:01:04
 * @ Modified time: 2024-03-31 01:59:31
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
 * @return 	{ int }							Whether or not the operation was successful.
*/
int Stats_update(Game *this) {
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
	char sGameGridData[GAME_MAX_ROWS][GAME_MAX_COLUMNS + 1];

	// The data in the profile file
	int nProfileDataLength = 0;
	int nProfileNewDataLength = 0;
	char *sProfileDataArray[PROFILE_FILE_MAX_HEIGHT + 1];
	char *sProfileNewDataArray[PROFILE_FILE_MAX_HEIGHT + 1];

	// Path to the profile
	sProfilePath = String_alloc(PROFILE_USERNAME_MAX_LENGTH + strlen(PROFILE_FOLDER_PATH) + 8);
	sprintf(sProfilePath, "%s%s.txt", PROFILE_FOLDER_PATH, this->pProfile->sCurrentProfile);
	
	// If file doesn't exist, exit
	if(!File_exists(sProfilePath))
		return 0;

	// New profile file
	pProfileFile = File_create(sProfilePath);

	// Read the file
	File_readText(pProfileFile, PROFILE_FILE_MAX_HEIGHT + 1, &nProfileDataLength, sProfileDataArray);
	nProfileNewDataLength = nProfileDataLength;

	// If too many lines in file
	if(this->field.dHeight + nProfileDataLength + 1 >= PROFILE_FILE_MAX_HEIGHT) {
		File_kill(pProfileFile);
		File_freeBuffer(nProfileDataLength, sProfileDataArray);
		
		return 0;
	}

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

	// The player lost
	if(this->eOutcome == GAME_OUTCOME_LOSS) {
		if(this->eType == GAME_TYPE_CUSTOM) nCustomLoss++;
		else if(this->eDifficulty == GAME_DIFFICULTY_EASY) nClassicEasyLoss++;
		else if(this->eDifficulty == GAME_DIFFICULTY_DIFFICULT) nClassicDifficultLoss++;

	// The player won
	} else if(this->eOutcome == GAME_OUTCOME_WIN) {
		if(this->eType == GAME_TYPE_CUSTOM) nCustomWins++;
		else if(this->eDifficulty == GAME_DIFFICULTY_EASY) nClassicEasyWins++;
		else if(this->eDifficulty == GAME_DIFFICULTY_DIFFICULT) nClassicDifficultWins++;
	}

	// Update the player stats
	sprintf(sClassicEasy, "-CLASSIC_EASY:%d,%d;\n", nClassicEasyWins, nClassicEasyLoss);
	sprintf(sClassicDifficult, "-CLASSIC_DIFFICULT:%d,%d;\n", nClassicDifficultWins, nClassicDifficultLoss);
	sprintf(sCustom, "-CUSTOM:%d,%d;\n", nCustomWins, nCustomLoss);

	// Write the new stats of the game
	sprintf(sGameData, ">%s,%s,%s,%d,%d;\n", 
		this->eType == GAME_TYPE_CLASSIC ? "CLASSIC" : "CUSTOM",
		this->eType == GAME_TYPE_CLASSIC ? String_toUpper(this->sSaveName) : 
			(this->eDifficulty == GAME_DIFFICULTY_EASY ? "EASY" : "DIFFICULT"),
		this->eOutcome == GAME_OUTCOME_WIN ? "WIN" : 
			(this->eOutcome == GAME_OUTCOME_LOSS ? "LOSS" : "QUIT"),
		
		this->field.dWidth, 
		this->field.dHeight);

	// Write the state of the board
	switch(this->eOutcome) {
		
		// The user won
		case GAME_OUTCOME_WIN:
			for(i = 0; i < this->field.dHeight; i++) {
				for(j = 0; j < this->field.dWidth; j++)
					sGameGridData[i][j] = 
						this->field.aNumbers[i][j] > -1 ? 
							(this->field.aNumbers[i][j] > 0 ? 
								this->field.aNumbers[i][j] + 48 : '.') : '*';		
				sGameGridData[i][j] = '\n';
			}
		break;

		// The user lost
		case GAME_OUTCOME_LOSS:
			for(i = 0; i < this->field.dHeight; i++) {
				for(j = 0; j < this->field.dWidth; j++)
					sGameGridData[i][j] = 
						this->field.aNumbers[i][j] > -1 ? 
							(this->field.aNumbers[i][j] > 0 ? 
								this->field.aNumbers[i][j] + 48 : '.') : '*';		
				sGameGridData[i][j] = '\n';
			}
		break;

		// The player quit the game
		default:
			for(i = 0; i < this->field.dHeight; i++) {
				for(j = 0; j < this->field.dWidth; j++)
					sGameGridData[i][j] = 
						Grid_getBit(this->field.pFlagGrid, j, i) ? '^' : 
							(Grid_getBit(this->field.pInspectGrid, j, i) ? 
								(this->field.aNumbers[i][j] > 0 ? this->field.aNumbers[i][j] + 48 : '.') : ' ');		
				sGameGridData[i][j] = '\n';
			}
		break;
	}

	// Create the header of the file again
	sProfileNewDataArray[0] = String_create(sClassicEasy);
	sProfileNewDataArray[1] = String_create(sClassicDifficult);
	sProfileNewDataArray[2] = String_create(sCustom);

	// Copy all the other lines
	for(i = PROFILE_FILE_HEADER_HEIGHT; i < nProfileDataLength; i++)
		sProfileNewDataArray[i] = String_create(sProfileDataArray[i]);
		
	// Add the data for that game
	sProfileNewDataArray[i] = String_create(sGameData);

	// Add in the new lines
	for(i++; i <= nProfileDataLength + this->field.dHeight + 1; i++) {
		sProfileNewDataArray[i] = String_create(sGameGridData[i - nProfileDataLength - 1]);
		nProfileNewDataLength++;
	}

	// Rewrite the file contents
	File_clear(pProfileFile);
	File_writeText(pProfileFile, nProfileNewDataLength, sProfileNewDataArray);

	// Garbage collection
	File_freeBuffer(nProfileDataLength, sProfileDataArray);
	File_freeBuffer(nProfileNewDataLength, sProfileNewDataArray);
}

#endif