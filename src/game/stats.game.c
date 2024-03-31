/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 17:01:04
 * @ Modified time: 2024-03-31 14:52:40
 * @ Modified time: 2024-03-31 15:10:37
 * 
 * Displays the statistics of a profile.
 */

#ifndef STATS_
#define STATS_

#include "./game.c"
#include "./profile.game.c"

#include "../utils/utils.string.h"

#include <stdio.h>

typedef struct Stats Stats;

struct Stats {
	
};

/**
 * Saves the information held by the most recent game.
 * 
 * @param		{ Game * }	pGame		The game object.
 * @return 	{ int }							Whether or not the operation was successful.
*/
int Stats_update(Game *this) {
	int i, j, k;

	// The profile file
	char *sProfilePath;
	File *pProfileFile;

	// The general stats
	int nClassicEasy[3] = { 0,  0, -1 };
	int nClassicDifficult[3] = { 0, 0, -1 };
	int nCustom[3] = { 0, 0, -1 };
	char sClassicEasy[32] = { 0 }, sClassicDifficult[32] = { 0 }, sCustom[32] = { 0 };

	// The latest game data
	char sGameData[256];
	char sGameGridData[GAME_MAX_ROWS + 1][GAME_MAX_COLUMNS + 2] = {0};

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

		// Skip the name
		k = 0;
		while(sProfileDataArray[i][++k] != ':');
		
		// For each parameter (wins, losses, best time)
		for(j = 0, k; j < 3; j++) {

			// Read the parameter
			String_clear(sClassicEasy);
			String_clear(sClassicDifficult);
			String_clear(sCustom);

			++k;
			while(sProfileDataArray[i][k] != ',' && 
				sProfileDataArray[i][k] != ';') {
				
				// Easy, Diff, or Custom
				switch(i) {
					case 0: sprintf(sClassicEasy, "%s%c", sClassicEasy, sProfileDataArray[i][k]); break;
					case 1: sprintf(sClassicDifficult, "%s%c", sClassicDifficult, sProfileDataArray[i][k]); break;
					case 2: sprintf(sCustom, "%s%c", sCustom, sProfileDataArray[i][k]); break;
					default: break;
				} ++k;
			}


			switch(i) {
				case 0: nClassicEasy[j] = atoi(sClassicEasy); break;
				case 1: nClassicDifficult[j] = atoi(sClassicDifficult); break;
				case 2: nCustom[j] = atoi(sCustom); break;
				default: break;
			}
		}
	}

	// The player lost
	if(this->eOutcome == GAME_OUTCOME_LOSS) {
		if(this->eType == GAME_TYPE_CUSTOM) nCustom[1]++;
		else if(this->eDifficulty == GAME_DIFFICULTY_EASY) nClassicEasy[1]++;
		else if(this->eDifficulty == GAME_DIFFICULTY_DIFFICULT) nClassicDifficult[1]++;

	// The player won
	} else if(this->eOutcome == GAME_OUTCOME_WIN) {
		if(this->eType == GAME_TYPE_CUSTOM) {
			nCustom[0]++;
			nCustom[2] = nCustom[2] > -1 ? min(nCustom[2], this->dTimeTaken) : this->dTimeTaken;
		} else if(this->eDifficulty == GAME_DIFFICULTY_EASY) {
			nClassicEasy[0]++;
			nClassicEasy[2] = nClassicEasy[2] > -1 ? min(nClassicEasy[2], this->dTimeTaken) : this->dTimeTaken;
		} else if(this->eDifficulty == GAME_DIFFICULTY_DIFFICULT) {
			nClassicDifficult[0]++;
			nClassicDifficult[2] = nClassicDifficult[2] > -1 ? min(nClassicDifficult[2], this->dTimeTaken) : this->dTimeTaken;
		};
	}

	// Update the player stats
	sprintf(sClassicEasy, "-CLASSIC_EASY:%d,%d,%d;\n", nClassicEasy[0], nClassicEasy[1], nClassicEasy[2]);
	sprintf(sClassicDifficult, "-CLASSIC_DIFFICULT:%d,%d,%d;\n", nClassicDifficult[0], nClassicDifficult[1], nClassicDifficult[2]);
	sprintf(sCustom, "-CUSTOM:%d,%d,%d;\n", nCustom[0], nCustom[1], nCustom[2]);

	// Write the new stats of the game
	sprintf(sGameData, ">%s,%s,%s,%d,%d,%d;\n", 
		this->eType == GAME_TYPE_CLASSIC ? "CLASSIC" : "CUSTOM",
		this->eType == GAME_TYPE_CLASSIC ? (this->eDifficulty == GAME_DIFFICULTY_EASY ? "EASY" : "DIFFICULT") : 
			String_toUpper(this->sSaveName),
		this->eOutcome == GAME_OUTCOME_WIN ? "WIN" : 
			(this->eOutcome == GAME_OUTCOME_LOSS ? "LOSS" : "QUIT"),
		
		this->field.dWidth, 
		this->field.dHeight,
		this->dTimeTaken);

	// Write the state of the board
	switch(this->eOutcome) {
		
		// The user won
		case GAME_OUTCOME_WIN:
			for(i = 0; i < this->field.dHeight; i++) {
				for(j = 0; j < this->field.dWidth; j++)
					sGameGridData[i][j] = 
						this->field.aNumbers[i][j] > -1 ? 
							(this->field.aNumbers[i][j] > 0 ? 
								this->field.aNumbers[i][j] + 48 : '.') : '^';		
				sGameGridData[i][j] = '\n';
			}
		break;

		// The user lost
		case GAME_OUTCOME_LOSS:
			for(i = 0; i < this->field.dHeight; i++) {
				for(j = 0; j < this->field.dWidth; j++)
					sGameGridData[i][j] = 
						Grid_getBit(this->field.pFlagGrid, j, i) ?
							(Grid_getBit(this->field.pMineGrid, j, i) ? '^' : 'X') :
								(Grid_getBit(this->field.pMineGrid, j, i) ? '*' : 
									(this->field.aNumbers[i][j] > 0 ? this->field.aNumbers[i][j] + 48 : '.'));	
				sGameGridData[i][j] = '\n';
			}

			// Exploded mine
			sGameGridData[this->dLastY][this->dLastX] = '@';
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

	return 1;
}

/**
 * Reads stats from the specified profile.
 * 
 * @param		{ Game * }	this	The game object.
*/
int Stats_read(Game *this) {
	
}

#endif