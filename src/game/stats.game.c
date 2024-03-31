/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 17:01:04
 * @ Modified time: 2024-03-31 15:30:50
 * @ Modified time: 2024-04-01 02:58:35
 * 
 * Displays the statistics of a profile.
 */

#ifndef STATS_
#define STATS_

#include "./game.c"
#include "./profile.game.c"

#include "../utils/utils.math.h"
#include "../utils/utils.string.h"

#include <stdio.h>
#include <time.h>

/**
 * Reads the top few lines of the profile file so we can use the data or smth.
 * 
 * @param		{ char * }	sProfileDataArray[]		The file contents we want to read from.
 * @param		{ int * }		nCustom								Where to store the data for custom games.
 * @param		{ int * }		nClassicEasy					Where to store the data for classic easy games.
 * @param		{ int * }		nClassicDifficult			Where to store the data for classic diff games.
*/
void Stats_readProfileHeader(char *sProfileDataArray[], int *nCustom, int *nClassicEasy, int *nClassicDifficult) {
	int i, j, k, l;

	char sClassicEasy[32] = { 0 };
	char sClassicDifficult[32] = { 0 };
	char sCustom[32] = { 0 };

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

			++k; l = 0;
			while(sProfileDataArray[i][k] != ',' && 
				sProfileDataArray[i][k] != ';') {
				
				// Easy, Diff, or Custom
				switch(i) {
					case 0: sClassicEasy[l++] = sProfileDataArray[i][k]; break;
					case 1: sClassicDifficult[l++] = sProfileDataArray[i][k]; break;
					case 2: sCustom[l++] = sProfileDataArray[i][k]; break;
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
}

/**
 * Reads stats from the specified profile and store them in memory.
 * 
 * @param		{ Profile * }		this	The game object.
*/
int Stats_readProfile(Profile *this) {
	int i, j, k, l;

	// The profile file
	char *sProfilePath;
	File *pProfileFile;

	// The general stats
	int nClassicEasy[3] = { 0,  0, -1 };
	int nClassicDifficult[3] = { 0, 0, -1 };
	int nCustom[3] = { 0, 0, -1 };
	int nTime = 0;

	// Entry data
	char sWordEntry[256] = { 0 };
	GameType eEntryType;
	GameDifficulty eEntryDifficulty;
	GameOutcome eEntryOutcome;

	// The data in the profile file
	int nProfileDataLength = 0;
	char *sProfileDataArray[PROFILE_FILE_MAX_HEIGHT + 1];

	// Path to the profile
	sProfilePath = String_alloc(PROFILE_USERNAME_MAX_LENGTH + strlen(PROFILE_FOLDER_PATH) + 8);
	sprintf(sProfilePath, "%s%s.txt", PROFILE_FOLDER_PATH, this->sCurrentProfile);

	// If file doesn't exist, exit
	if(!File_exists(sProfilePath))
		return 0;

	// New profile file
	pProfileFile = File_create(sProfilePath);

	// Read the file
	File_readText(pProfileFile, PROFILE_FILE_MAX_HEIGHT + 1, &nProfileDataLength, sProfileDataArray);

	// Read the file header
	Stats_readProfileHeader(sProfileDataArray, nCustom, nClassicEasy, nClassicDifficult);

	// Set the stuff in memory
	for(i = 0; i < 3; i++) {
		this->dCustomStats[i] = nCustom[i];
		this->dClassicEasyStats[i] = nClassicEasy[i];
		this->dClassicDifficultStats[i] = nClassicDifficult[i];
	}

	// Set these to 0 first
	this->nCustom = 0;
	this->nClassicEasy = 0;
	this->nClassicDifficult = 0;
	this->nHistoryHeight = 0;

	// Parse the file and copy to memory
	for(i = PROFILE_FILE_HEADER_HEIGHT; i < nProfileDataLength; i++) {
		this->sHistory[this->nHistoryHeight] = String_create(sProfileDataArray[i]);

		// Start of a new board entry	
		if(this->sHistory[this->nHistoryHeight][0] == '>') {
			j = 0, k = 0;

			// Parse the words in the entry
			while(this->sHistory[this->nHistoryHeight][j] != ';') {
				String_clear(sWordEntry);

				// Copy the next word
				++j; l = 0;
				while(this->sHistory[this->nHistoryHeight][j] != ',' && 
					this->sHistory[this->nHistoryHeight][j] != ';')
					sWordEntry[l++] = this->sHistory[this->nHistoryHeight][j++];
				
				// Which entry we're at
				switch(k) {

					// Set the game type
					case 0:
						if(!strcmp(sWordEntry, "CUSTOM"))	eEntryType = GAME_TYPE_CUSTOM;
						else if(!strcmp(sWordEntry, "CLASSIC")) eEntryType = GAME_TYPE_CLASSIC;
					break;

					// Set game difficult
					case 1:
						if(!strcmp(sWordEntry, "EASY"))	eEntryDifficulty = GAME_DIFFICULTY_EASY;
						else if(!strcmp(sWordEntry, "DIFFICULT")) eEntryDifficulty = GAME_DIFFICULTY_DIFFICULT;
						else eEntryDifficulty = GAME_DIFFICULTY_NONE;
					break;	

					// Set game outcome
					case 2:
						if(!strcmp(sWordEntry, "WIN"))	eEntryOutcome = GAME_OUTCOME_WIN;
						else if(!strcmp(sWordEntry, "LOSS")) eEntryOutcome = GAME_OUTCOME_LOSS;
						else if(!strcmp(sWordEntry, "QUIT")) eEntryOutcome = GAME_OUTCOME_QUIT;
					break;

					case 3: break;	// width
					case 4: break;	// height
					case 5: nTime = atoi(sWordEntry); break;
					case 6: break;	// timestamp
					default: break;
				}
				++k;
			}

		
			// Update the profile stats
			// Custom
			if(eEntryType == GAME_TYPE_CUSTOM) {
				if(eEntryOutcome == GAME_OUTCOME_WIN)
					this->dCustomTimes[this->nCustom++] = nTime;
				else if(eEntryOutcome == GAME_OUTCOME_LOSS)
					this->dCustomTimes[this->nCustom++] = -1;
				else
					this->dCustomTimes[this->nCustom++] = -2;

			} else if(eEntryType == GAME_TYPE_CLASSIC) {
				
				// Classic easy
				if(eEntryDifficulty == GAME_DIFFICULTY_EASY) {
					if(eEntryOutcome == GAME_OUTCOME_WIN)
						this->dClassicEasyTimes[this->nClassicEasy++] = nTime;
					else if(eEntryOutcome == GAME_OUTCOME_LOSS)
						this->dClassicEasyTimes[this->nClassicEasy++] = -1;
					else
						this->dClassicEasyTimes[this->nClassicEasy++] = -2;
						
				// Classic difficult
				} else if(eEntryDifficulty == GAME_DIFFICULTY_DIFFICULT) {
					if(eEntryOutcome == GAME_OUTCOME_WIN)
						this->dClassicDifficultTimes[this->nClassicDifficult++] = nTime;
					else if(eEntryOutcome == GAME_OUTCOME_LOSS)
						this->dClassicDifficultTimes[this->nClassicDifficult++] = -1;
					else
						this->dClassicDifficultTimes[this->nClassicDifficult++] = -2;
				}
			}
		}

		// Increment the height
		this->nHistoryHeight++;
	}

	// Almost forgot!
	File_kill(pProfileFile);
	File_freeBuffer(nProfileDataLength, sProfileDataArray);

	return 1;
}

/**
 * Saves the information held by the most recent game.
 * 
 * @param		{ Game * }			this			The game object.
 * @return 	{ int }										Whether or not the operation was successful.
*/
int Stats_saveGame(Game *this) {
	int i, j, k;
	Profile *pProfile = this->pProfile;

	// The profile file
	char *sProfilePath;
	File *pProfileFile;

	// The general stats
	int nClassicEasy[3] = { 0,  0, -1 };
	int nClassicDifficult[3] = { 0, 0, -1 };
	int nCustom[3] = { 0, 0, -1 };

	char sClassicEasy[32] = { 0 };
	char sClassicDifficult[32] = { 0 };
	char sCustom[32] = { 0 };

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

	// Too many games
	if(pProfile->nClassicEasy + 1 > PROFILES_MAX_GAMES ||
		pProfile->nClassicDifficult + 1 > PROFILES_MAX_GAMES ||
		pProfile->nCustom + 1 > PROFILES_MAX_GAMES) {
			return 0;
	}
	
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
	Stats_readProfileHeader(sProfileDataArray, nCustom, nClassicEasy, nClassicDifficult);

	// The player lost
	if(this->eOutcome == GAME_OUTCOME_LOSS) {
		if(this->eType == GAME_TYPE_CUSTOM) nCustom[1]++;
		else if(this->eDifficulty == GAME_DIFFICULTY_EASY) nClassicEasy[1]++;
		else if(this->eDifficulty == GAME_DIFFICULTY_DIFFICULT) nClassicDifficult[1]++;

	// The player won
	} else if(this->eOutcome == GAME_OUTCOME_WIN) {
		if(this->eType == GAME_TYPE_CUSTOM) {
			nCustom[0]++;
			nCustom[2] = nCustom[2] > -1 ? minInt(nCustom[2], this->dTimeTaken) : this->dTimeTaken;
			pProfile->dCustomStats[2] = nCustom[2];

		} else if(this->eDifficulty == GAME_DIFFICULTY_EASY) {
			nClassicEasy[0]++;
			nClassicEasy[2] = nClassicEasy[2] > -1 ? minInt(nClassicEasy[2], this->dTimeTaken) : this->dTimeTaken;
			pProfile->dClassicEasyStats[2] = nClassicEasy[2];

		} else if(this->eDifficulty == GAME_DIFFICULTY_DIFFICULT) {
			nClassicDifficult[0]++;
			nClassicDifficult[2] = nClassicDifficult[2] > -1 ? minInt(nClassicDifficult[2], this->dTimeTaken) : this->dTimeTaken;
			pProfile->dClassicDifficultStats[2] = nClassicDifficult[2];
		};
	}

	// Update the things in memory too
	if(this->eType == GAME_TYPE_CUSTOM)
		pProfile->dCustomTimes[pProfile->nCustom++] = 
			this->eOutcome == GAME_OUTCOME_WIN ? this->dTimeTaken : 
				(this->eOutcome == GAME_OUTCOME_LOSS ? -1 : -2);

	else if(this->eDifficulty == GAME_DIFFICULTY_EASY)
		pProfile->dClassicEasyTimes[pProfile->nClassicEasy++] = 
			this->eOutcome == GAME_OUTCOME_WIN ? this->dTimeTaken : 
				(this->eOutcome == GAME_OUTCOME_LOSS ? -1 : -2);
	
	else if(this->eDifficulty == GAME_DIFFICULTY_DIFFICULT)
		pProfile->dClassicDifficultTimes[pProfile->nClassicDifficult++] = 
			this->eOutcome == GAME_OUTCOME_WIN ? this->dTimeTaken : 
				(this->eOutcome == GAME_OUTCOME_LOSS ? -1 : -2);

	// Update the player stats
	sprintf(sClassicEasy, "-CLASSIC_EASY:%d,%d,%d;\n", nClassicEasy[0], nClassicEasy[1], nClassicEasy[2]);
	sprintf(sClassicDifficult, "-CLASSIC_DIFFICULT:%d,%d,%d;\n", nClassicDifficult[0], nClassicDifficult[1], nClassicDifficult[2]);
	sprintf(sCustom, "-CUSTOM:%d,%d,%d;\n", nCustom[0], nCustom[1], nCustom[2]);

	// Write the new stats of the game
	sprintf(sGameData, ">%s,%s,%s,%d,%d,%d,%s;\n", 
		this->eType == GAME_TYPE_CLASSIC ? "CLASSIC" : "CUSTOM",
		this->eType == GAME_TYPE_CLASSIC ? (this->eDifficulty == GAME_DIFFICULTY_EASY ? "EASY" : "DIFFICULT") : 
			String_toUpper(this->sSaveName),
		this->eOutcome == GAME_OUTCOME_WIN ? "WIN" : 
			(this->eOutcome == GAME_OUTCOME_LOSS ? "LOSS" : "QUIT"),
		
		this->field.dWidth, this->field.dHeight,
		this->dTimeTaken, this->sTimestamp);

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
	pProfile->sHistory[pProfile->nHistoryHeight++] = String_create(sGameData);

	// Add in the new lines to both the file and the memory
	for(i++; i <= nProfileDataLength + this->field.dHeight + 1; i++) {
		sProfileNewDataArray[i] = String_create(sGameGridData[i - nProfileDataLength - 1]);
		pProfile->sHistory[pProfile->nHistoryHeight++] = String_create(sGameGridData[i - nProfileDataLength - 1]);
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
 * Clears the stats and does garbage collection on the profile before loading a new one.
 * 
 * @param		{ Profile * }		this	The game object.
*/
int Stats_clearProfile(Profile *this) {
	int i;

	if(this->nHistoryHeight)
		while(--this->nHistoryHeight)
			String_kill(this->sHistory[this->nHistoryHeight]);

	this->nCustom = 0;
	this->nClassicEasy = 0;
	this->nClassicDifficult = 0;
}

/**
 * Gets how many games the profile has won in the specified category.
 * 
 * @param		{ Profile * }				this					The profile object.
 * @param		{ GameType }				eType					The type of game we're looking stats for.
 * @param		{ GameDifficulty }	eDifficulty		The difficulty of the game, if applicable.
 * @return	{ int }														How many games they won.
*/
int Stats_getWins(Profile *this, GameType eType, GameDifficulty eDifficulty) {
	int i, count = 0;

	if(eType == GAME_TYPE_CLASSIC) {
		
		// Classic easy
		if(eDifficulty == GAME_DIFFICULTY_EASY) {
			for(i = 0; i < this->nClassicEasy; i++)
				if(this->dClassicEasyTimes[i] > -1)
					count++;

		// Classic difficult
		} else if(eDifficulty == GAME_DIFFICULTY_DIFFICULT) {
			for(i = 0; i < this->nClassicDifficult; i++)
				if(this->dClassicDifficultTimes[i] > -1)
					count++;	
		}

	// Custom mode
	} else {
		for(i = 0; i < this->nCustom; i++)
			if(this->dCustomTimes[i] > -1)
				count++;
	}

	return count;
}

/**
 * Gets how many games the profile has lost in the specified category.
 * 
 * @param		{ Profile * }				this					The profile object.
 * @param		{ GameType }				eType					The type of game we're looking stats for.
 * @param		{ GameDifficulty }	eDifficulty		The difficulty of the game, if applicable.
 * @return	{ int }														How many games they lost.
*/
int Stats_getLosses(Profile *this, GameType eType, GameDifficulty eDifficulty) {
	int i, count = 0;

	if(eType == GAME_TYPE_CLASSIC) {
		
		// Classic easy
		if(eDifficulty == GAME_DIFFICULTY_EASY) {
			for(i = 0; i < this->nClassicEasy; i++)
				if(this->dClassicEasyTimes[i] == -1)
					count++;

		// Classic difficult
		} else if(eDifficulty == GAME_DIFFICULTY_DIFFICULT) {
			for(i = 0; i < this->nClassicDifficult; i++)
				if(this->dClassicDifficultTimes[i] == -1)
					count++;	
		}

	// Custom mode
	} else {
		for(i = 0; i < this->nCustom; i++)
			if(this->dCustomTimes[i] == -1)
				count++;
	}

	return count;
}

/**
 * Gets how many games the profile has quit in the specified category.
 * 
 * @param		{ Profile * }				this					The profile object.
 * @param		{ GameType }				eType					The type of game we're looking stats for.
 * @param		{ GameDifficulty }	eDifficulty		The difficulty of the game, if applicable.
 * @return	{ int }														How many games they quit.
*/
int Stats_getQuits(Profile *this, GameType eType, GameDifficulty eDifficulty) {
	int i, count = 0;

	if(eType == GAME_TYPE_CLASSIC) {
		
		// Classic easy
		if(eDifficulty == GAME_DIFFICULTY_EASY) {
			for(i = 0; i < this->nClassicEasy; i++)
				if(this->dClassicEasyTimes[i] == -2)
					count++;

		// Classic difficult
		} else if(eDifficulty == GAME_DIFFICULTY_DIFFICULT) {
			for(i = 0; i < this->nClassicDifficult; i++)
				if(this->dClassicDifficultTimes[i] == -2)
					count++;	
		}

	// Custom mode
	} else {
		for(i = 0; i < this->nCustom; i++)
			if(this->dCustomTimes[i] == -2)
				count++;
	}

	return count;
}

/**
 * Gets how many games the profile has played in the specified category.
 * 
 * @param		{ Profile * }				this					The profile object.
 * @param		{ GameType }				eType					The type of game we're looking stats for.
 * @param		{ GameDifficulty }	eDifficulty		The difficulty of the game, if applicable.
 * @return	{ int }														How many games they played.
*/
int Stats_getTotalGames(Profile *this, GameType eType, GameDifficulty eDifficulty) {
	if(eType == GAME_TYPE_CLASSIC) {
		
		// Classic easy
		if(eDifficulty == GAME_DIFFICULTY_EASY) {
			return this->nClassicEasy;

		// Classic difficult
		} else if(eDifficulty == GAME_DIFFICULTY_DIFFICULT) {
			return this->nClassicDifficult;
		}

		// Yeppers
		return 0;

	// Custom mode
	} else {
		return this->nCustom;
	}
}

/**
 * Gets the best time acquired by the player in that category.
 * 
 * @param		{ Profile * }				this					The profile object.
 * @param		{ GameType }				eType					The type of game we're looking stats for.
 * @param		{ GameDifficulty }	eDifficulty		The difficulty of the game, if applicable.
 * @return	{ int }														The best time they had for that game.
*/
int Stats_getBest(Profile *this, GameType eType, GameDifficulty eDifficulty) {
	if(eType == GAME_TYPE_CLASSIC) {
		
		// Classic easy
		if(eDifficulty == GAME_DIFFICULTY_EASY) {
			return this->dClassicEasyStats[2];

		// Classic difficult
		} else if(eDifficulty == GAME_DIFFICULTY_DIFFICULT) {
			return this->dClassicDifficultStats[2];
		}

		// Yeppers
		return -1;

	// Custom mode
	} else {
		return this->dCustomStats[2];
	}
}

/**
 * Outputs the requested board to the text buffer.
 * Note that 0 represents the most recent board, 1 the second most recent, etc.
 * Returns a 0 when the end of the history was reached and the board wasn't found (n was too large).
 * 
 * @param		{ Profile * }		this 						The profile object.
 * @param		{ int }					n								The nth board (0th -> most recent).
 * @param		{ int * }				nHeight					The height of the board.
 * @param		{ char ** }			sOutputBuffer		The output buffer.
 * @return	{ int }													Whether or not the operation was successful.
*/
int Stats_getBoard(Profile *this, int n, int *nHeight, char **sOutputBuffer) {
	int i = this->nHistoryHeight, j, k;
	char sWordEntry[256] = { 0 };

	// No boards yet
	if(!this->nHistoryHeight)
		return 0;

	// Go through the entire history first and look for the start of the board
	while(i > 0 && n >= 0) {
		if(this->sHistory[--i][0] == '>')
			n--;
	}

	// Make sure we didn't hit the last entry without having reached the nth
	if(i <= 0 && ++n > 0)
		return 0;

	// Init for the loop below
	*nHeight = 0; 

	// Parse the description of the board first
	j = 0;
	while(this->sHistory[i][0] == '>' && 
		this->sHistory[i][j] != ';') {
		String_clear(sWordEntry);
		
		j++; k = 0;
		while(this->sHistory[i][j] != ',' && 
			this->sHistory[i][j] != ';')
			sWordEntry[k++] = this->sHistory[i][j++];

		sOutputBuffer[*nHeight] = String_create(sWordEntry);
		(*nHeight)++;
	}
	
	// While we haven't hit the next board
	i++;
	while(i < this->nHistoryHeight) {
		if(this->sHistory[i][0] == '>')
			return 1;
			
		sOutputBuffer[*nHeight] = String_create(this->sHistory[i]);
		(*nHeight)++;
		i++;
	}

	return 1;
}

#endif