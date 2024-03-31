/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-27 2:13:51
 * @ Modified time: 2024-04-01 06:46:43
 * @ Description:
 * 
 * Handles the current profile managed by the game.
 */

#ifndef PROFILE_
#define PROFILE_

#include "game.c"

#include "../utils/utils.file.h"
#include "../utils/utils.string.h"

#include <stdio.h>
#include <string.h>


#define PROFILES_FILE_PATH "./src/data/profiles/profiles.data.txt"
#define PROFILE_FOLDER_PATH "./src/data/profiles/"

#define PROFILE_FILE_HEADER_HEIGHT 3
#define PROFILE_FILE_MAX_HEIGHT (1 << 16)

#define PROFILES_MAX_NUM 10
#define PROFILES_MAX_GAMES (1 << 12)
#define PROFILE_PASSWORD_MAX_LENGTH 32
#define PROFILE_USERNAME_MAX_LENGTH 20
#define PROFILE_PASSWORD_MIN_LENGTH 3
#define PROFILE_USERNAME_MIN_LENGTH 3

typedef enum ProfileError ProfileError;
typedef struct Profile Profile;

enum ProfileError {
	PROFILE_ERROR_NONE,										// No error
	PROFILE_ERROR_NO_FILE,								// Profiles file does not exist
	PROFILE_ERROR_NOT_FOUND,							// Logging in into a non-existent profile
	PROFILE_ERROR_ALREADY_EXISTS,					// Attempting to create an existing profile
	PROFILE_ERROR_INVALID_LENGTH,					// Password or username had invalid length
	PROFILE_ERROR_INVALID_CHARS,					// Password or username has invalid chars
	PROFILE_ERROR_INVALID_LOGIN,					// Incorrect password for existing profile
	PROFILE_ERROR_TOO_MANY_EXISTING,			// Too many profiles exist already
	PROFILE_ERROR_COULD_NOT_CREATE_FILE,	// The new file could not be created for the profile
	PROFILE_ERROR_FILE_ALREADY_DELETED,		// Trying to delete a nonexistent file
};

/**
 * We pass this object to the engine.
*/
struct Profile {
  char sCurrentProfile[PROFILE_USERNAME_MAX_LENGTH + 1];

	// Profile stats
	int dCustomStats[3];						// Wins, losses, best time (in seconds)
	int dClassicEasyStats[3];
	int dClassicDifficultStats[3];

	// Game time histories
	int dCustomTimes[PROFILES_MAX_GAMES], nCustom;
	int dClassicEasyTimes[PROFILES_MAX_GAMES], nClassicEasy;
	int dClassicDifficultTimes[PROFILES_MAX_GAMES], nClassicDifficult;

	// Game board histories
	int nHistoryHeight;
	char *sHistory[PROFILE_FILE_MAX_HEIGHT];

	// Last encountered error
	ProfileError eError;
};

/**
 * Initializes the profile object.
 * 
 * @param		{ Profile * }		this	The profile object.
*/
void Profile_init(Profile *this) {
  strcpy(this->sCurrentProfile, "DEV");
	this->eError = PROFILE_ERROR_NONE;

	this->dCustomStats[0] = 0;
	this->dClassicEasyStats[0] = 0;
	this->dClassicDifficultStats[0] = 0;

	this->dCustomStats[1] = 0;
	this->dClassicEasyStats[1] = 0;
	this->dClassicDifficultStats[1] = 0;

	this->dCustomStats[2] = -1;
	this->dClassicEasyStats[2] = -1;
	this->dClassicDifficultStats[2] = -1;
	
	this->nCustom = 0;
	this->nClassicEasy = 0;
	this->nClassicDifficult = 0;

	// Clear history
	this->nHistoryHeight = 0;
}

/**
 * Attempts to log into a profile given the credentials.
 * This is some weak ass password storing (plain text bruh).
 * 
 * @param		{ Profile * }		this				The profile object.
 * @param		{ char * }			sUsername		The username provided.
 * @param		{ char * }			sPassword		The password provided.
 * @return	{ int }											Whether or not the operation wass successful.
*/
int Profile_login(Profile *this, char *sUsername, char *sPassword) {
	int i, j, k;
	File *pProfilesFile;

	// Store the profiles read from the file
	int nProfileCount = 0;
	char sProfileUsername[PROFILE_USERNAME_MAX_LENGTH + 1] = { 0 };
	char sProfilePassword[PROFILE_PASSWORD_MAX_LENGTH + 1] = { 0 };
	char *sProfilesArray[PROFILES_MAX_NUM];

	// No data to cross check login
	if(!File_exists(PROFILES_FILE_PATH)) {
		this->eError = PROFILE_ERROR_NO_FILE;
		return 0;
	}

	// Open the file
	pProfilesFile = File_create(PROFILES_FILE_PATH);

	// Read the text onto the output buffer
	File_readText(pProfilesFile, PROFILES_MAX_NUM + 1, &nProfileCount, sProfilesArray);

	// The profile exists
	for(i = 0; i < nProfileCount; i++) {
		String_clear(sProfileUsername);
		String_clear(sProfilePassword);
		
		// Copy the name first without the newline
		j = 0; k = 0;
		while(sProfilesArray[i][j] != ';' && sProfilesArray[i][j])
			sProfileUsername[k++] = sProfilesArray[i][j++];

		// Copy the password
		j++; k = 0;
		while(sProfilesArray[i][j] != ';' && sProfilesArray[i][j])
			sProfilePassword[k++] = sProfilesArray[i][j++];

		// Compare the name
		if(!strcmp(sProfileUsername, sUsername)) {
			
			// Correct login
			if(!strcmp(sProfilePassword, sPassword)) {
				i = nProfileCount + 1;

			// Invalid login
			} else {
				File_kill(pProfilesFile);
				File_freeBuffer(nProfileCount, sProfilesArray);
				this->eError = PROFILE_ERROR_INVALID_LOGIN;
				return 0;
			}
		}
	}

	// Profile waasn't found
	if(i == nProfileCount) {
		File_kill(pProfilesFile);
		File_freeBuffer(nProfileCount, sProfilesArray);
		this->eError = PROFILE_ERROR_NOT_FOUND;
		return 0;
	}

	// Set the current profile to the logged in username
	strcpy(this->sCurrentProfile, sUsername);

	// Garbage collection
	File_kill(pProfilesFile);
	File_freeBuffer(nProfileCount, sProfilesArray);
	
	// Successful
	return 1;
}

/**
 * Sets the active profile to the one indicated.
 * 
 * @param		{ Profile * }		this		The profile object to modify.
 * @param   { char * }  		sName   Name of the to-be-created profile.
 * @return	{ int }									Returns whether or not the operation was successful.
*/
int Profile_register(Profile *this, char *sUsername, char *sPassword) {
	int i, j;
	File *pProfilesFile;
	File *pProfileFile;
	
	// Stuff about the new profile
	char *sNewProfile[1];
	char *sNewProfilePath;
	char *sNewProfileData[PROFILE_FILE_HEADER_HEIGHT] = {
		"-CLASSIC_EASY:0,0,-1;\n",
		"-CLASSIC_DIFFICULT:0,0,-1;\n",
		"-CUSTOM:0,0,-1;\n",
	};

	// Stuff about existing profiles
	int nProfileCount = 0;
	char sProfileEntry[PROFILE_USERNAME_MAX_LENGTH + 1];
	char *sProfilesArray[PROFILES_MAX_NUM + 1];

	// Name was of invalid size
	if(strlen(sUsername) < PROFILE_USERNAME_MIN_LENGTH || strlen(sUsername) > PROFILE_USERNAME_MAX_LENGTH) {
		this->eError = PROFILE_ERROR_INVALID_LENGTH;
		return 0;
	}

	// Password was too long or too short
	if(strlen(sPassword) < PROFILE_PASSWORD_MIN_LENGTH || strlen(sPassword) > PROFILE_PASSWORD_MAX_LENGTH) {
		this->eError = PROFILE_ERROR_INVALID_LENGTH;
		return 0;
	}

	// Checks if the name's characters are only capital letters
	i = 0;
	do {
		if(sUsername[i] < 'A' || sUsername[i] > 'Z') {
			this->eError = PROFILE_ERROR_INVALID_CHARS;
			return 0;
		}
	} while(sUsername[++i]);

	// Checks if the password is also just all caps
	i = 0;
	do {
		if(sPassword[i] < 'A' || sPassword[i] > 'Z') {
			this->eError = PROFILE_ERROR_INVALID_CHARS;
			return 0;
		}
	} while(sPassword[++i]);

	// Set the current profile to the given name
	sNewProfile[0] = String_alloc(strlen(sUsername) + strlen(sPassword) + 4);
	strcpy(sNewProfile[0], sUsername);

	// Creates the text file with the list of profiles
	// Returns 0 if unsuccessful
	if(!File_exists(PROFILES_FILE_PATH)) {
		if(!File_newFile(PROFILES_FILE_PATH)) {
			this->eError = PROFILE_ERROR_NO_FILE;
			return 0;
		}
	}

	// Open the file
	pProfilesFile = File_create(PROFILES_FILE_PATH);

	// Read the text onto the output buffer
	File_readText(pProfilesFile, PROFILES_MAX_NUM + 1, &nProfileCount, sProfilesArray);

	// Too many profiles exist
	if(nProfileCount + 1 > PROFILES_MAX_NUM) {
		File_kill(pProfilesFile);
		File_freeBuffer(nProfileCount, sProfilesArray);
		String_kill(sNewProfile[0]);
		
		this->eError = PROFILE_ERROR_TOO_MANY_EXISTING;
		return 0;
	}

	// The profile already exists
	for(i = 0; i < nProfileCount; i++) {
		strcpy(sProfileEntry, "");

		// Copy the name first without the newline
		j = 0;
		while(sProfilesArray[i][j] != ';' && sProfilesArray[i][j]) {
			sProfileEntry[j] = sProfilesArray[i][j];
			j++;
		}
		
		// Compare the name
		if(!strcmp(sProfileEntry, sUsername)) {
			File_kill(pProfilesFile);
			File_freeBuffer(nProfileCount, sProfilesArray);
			String_kill(sNewProfile[0]);
			
			this->eError = PROFILE_ERROR_ALREADY_EXISTS;
			return 0;
		}
	}

	// Create a new file for that user
	sNewProfilePath = String_alloc(PROFILE_USERNAME_MAX_LENGTH + strlen(PROFILE_FOLDER_PATH) + 8);
	sprintf(sNewProfilePath, "%s%s.txt", PROFILE_FOLDER_PATH, sUsername);
	
	// If file could not be made
	if(!File_newFile(sNewProfilePath)) {
		File_kill(pProfilesFile);
		File_freeBuffer(nProfileCount, sProfilesArray);
		String_kill(sNewProfile[0]);
		String_kill(sNewProfilePath);
		
		this->eError = PROFILE_ERROR_COULD_NOT_CREATE_FILE;
		return 0;
	}

	// Write the seed data unto the file
	pProfileFile = File_create(sNewProfilePath);
	File_writeText(pProfileFile, PROFILE_FILE_HEADER_HEIGHT, sNewProfileData);

	// Otherwise, append the profile to the file
	strcat(sNewProfile[0], ";");
	strcat(sNewProfile[0], sPassword);
	strcat(sNewProfile[0], ";\n");
	File_writeText(pProfilesFile, 1, sNewProfile);

	// Garbage collection
	File_kill(pProfilesFile);
	File_freeBuffer(nProfileCount, sProfilesArray);
	String_kill(sNewProfile[0]);
	String_kill(sNewProfilePath);

	// Successful
	return 1;
}

/**
 * Deletes a profile.
 * 
 * @param		{ Profile * }		this				The profile object.
 * @param   { char * }  		sUsername		Name of the to-be-selected profile.
*/
int Profile_delete(Profile *this, char *sUsername) {
	int i, j;
	File *pProfilesFile;

	// Stuff about the current username
	char *sProfilePath;
	char sProfileUsername[PROFILE_USERNAME_MAX_LENGTH + 1];
	
	// Store the profiles read from the file
	int nProfileCount = 0;
	char *sProfileEntry[1];
	char *sProfilesArray[PROFILES_MAX_NUM + 1];

	// No data to cross check login
	if(!File_exists(PROFILES_FILE_PATH)) {
		this->eError = PROFILE_ERROR_NO_FILE;
		return 0;
	}

	// Open the file
	pProfilesFile = File_create(PROFILES_FILE_PATH);

	// Read the text onto the output buffer
	File_readText(pProfilesFile, PROFILES_MAX_NUM + 1, &nProfileCount, sProfilesArray);

	// Clear the file
	File_clear(pProfilesFile);

	// The profile exists
	for(i = 0; i < nProfileCount; i++) {
		String_clear(sProfileUsername);
		sProfileEntry[0] = String_create(sProfilesArray[i]);
		
		// Copy the name first without the newline
		j = 0; 
		while(sProfilesArray[i][j] != ';' && sProfilesArray[i][j] && 
			sProfilesArray[i][j] != '\n' && sProfilesArray[i][j] != '\r') {
			sProfileUsername[j] = sProfilesArray[i][j];
			j++;
		}

		// Copy to file only if not the selected profile
		if(strcmp(sProfileUsername, sUsername))
			File_writeText(pProfilesFile, 1, sProfileEntry);

		// Kill the allocated string
		String_kill(sProfileEntry[0]);
	}

	// Delete the file for the user
	sProfilePath = String_alloc(PROFILE_USERNAME_MAX_LENGTH + strlen(PROFILE_FOLDER_PATH) + 15);
	sprintf(sProfilePath, "%s%s.txt", PROFILE_FOLDER_PATH, sUsername);
	
	// Try to delete file, throw error otherwise
	if(!File_remove(sProfilePath)) {
		File_kill(pProfilesFile);
		File_freeBuffer(nProfileCount, sProfilesArray);
		this->eError = PROFILE_ERROR_FILE_ALREADY_DELETED;
		return 0;
	}

	// Set the current profile to an empty string
	strcpy(this->sCurrentProfile, "");

	// Garbage collection
	File_kill(pProfilesFile);
	File_freeBuffer(nProfileCount, sProfilesArray);
	
	// Successful
	return 1;
}

/**
 * Returns the list of all profiles unto an output buffer.
 *
 * @param 	{ int }				n								How many profiles were outputted.
 * @param		{ char ** }		sOutputBuffer		Where we store the profile names.
*/
void Profile_getList(int *n, char **sOutputBuffer) {
	int i, j;
	char sTemp[PROFILE_USERNAME_MAX_LENGTH + 1];

	// Stuff about the profiles file
	File *pProfilesFile;
	int nProfileCount;
	char *sProfilesArray[PROFILES_MAX_NUM];

	// Open the file
	pProfilesFile = File_create(PROFILES_FILE_PATH);

	// Read the text onto the output buffer
	File_readText(pProfilesFile, PROFILES_MAX_NUM + 1, &nProfileCount, sProfilesArray);

	// Copy them unto the output buffer
	for(i = 0, *n = 0; i < nProfileCount; i++) {
		sOutputBuffer[i] = String_alloc(PROFILE_USERNAME_MAX_LENGTH + 1);

		j = 0;
		while(sProfilesArray[i][j] != ';') {
			sOutputBuffer[i][j] = sProfilesArray[i][j];
			j++;
		}
	}

	// Sort the array
	for(i = 0; i < nProfileCount; i++) {
		for(j = i + 1; j < nProfileCount; j++) {
			
			// Swap
			if(strcmp(sOutputBuffer[i], sOutputBuffer[j]) > 0) {
				strcpy(sTemp, sOutputBuffer[i]);
				strcpy(sOutputBuffer[i], sOutputBuffer[j]);
				strcpy(sOutputBuffer[j], sTemp);
			}
		}
	}

	*n = i;
}

/**
 * Returns a description of the last error encountered by the profile object.
 * 
 * @param		{ Profile * }		this	The profile object.
 * @return  { char * }						A string that describes the current error of the profile object.
*/
char *Profile_getErrorMessage(Profile *this) {
	
	// Switch between the different possible errors
	switch(this->eError) {

		case PROFILE_ERROR_NO_FILE:
			return "Error: profiles.data.txt does not exist.";

		case PROFILE_ERROR_NOT_FOUND:
			return "Error: the account does not exist.";

		case PROFILE_ERROR_INVALID_LOGIN:
			return "Error: incorrect password.";

		case PROFILE_ERROR_INVALID_LENGTH:
			return "Error: username/password is too long or too short.";

		case PROFILE_ERROR_INVALID_CHARS:
			return "Error: username/password can only be all caps.";

		case PROFILE_ERROR_ALREADY_EXISTS:
			return "Error: the profile already exists.";

		case PROFILE_ERROR_TOO_MANY_EXISTING:
			return "Error: too many existing profiles.";

		case PROFILE_ERROR_COULD_NOT_CREATE_FILE:
			return "Error: could not create file for profile.";

		case PROFILE_ERROR_FILE_ALREADY_DELETED:
			return "Error: the profile file no longer exists.";

		default:
			return "Error: no error?";
	}
}

/**
 * Returns the actual enum representing the latest error.
 * 
 * @param		{ Profile * }			this	The profile object.
 * @return	{ ProfileError }				What error was last encountered.	
*/
ProfileError Profile_getErrorId(Profile *this) {
	return this->eError;
}

/**
 * Sets the error stored by the profile object.
 * 
 * @param		{ Profile * }			this		The profile object.
 * @return	{ ProfileError }	eError	What error to set.	
*/
void Profile_setErrorId(Profile *this, ProfileError eError) {
	this->eError = eError;
}

/**
 * Returns the name of the currently active profile.
 * 
 * @param		{ Profile * }		this	The profile object.
 * @return	{ char * }						The name of the active user.
*/
char *Profile_getCurrent(Profile *this) {
	return this->sCurrentProfile;
}

#endif