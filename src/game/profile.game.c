/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-27 2:13:51
 * @ Modified time: 2024-03-30 00:35:22
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


#define PROFILES_FILE_PATH "./src/data/profiles.data.txt"
#define GAME_FILE_PATH  "./src/data/game.data.txt"
#define PROFILE_FOLDER_PATH "./src/data/profiles/"
#define PROFILE_FOLDER_PATH_LENGTH strlen(PROFILE_FOLDER_PATH)

// +4 includes ".txt"
#define PROFILE_FILE_PATH_MAX_LENGTH (PROFILE_FOLDER_PATH_LENGTH + PROFILE_USERNAME_MAX_LENGTH + 4)
#define PROFILE_FILE_PATH_MAX_SIZE sizeof(char)*(PROFILE_FILE_PATH_MAX_LENGTH + 1)

#define PROFILES_MAX_NUM 10
#define PROFILE_PASSWORD_MAX_LENGTH 32
#define PROFILE_USERNAME_MAX_LENGTH 20
#define PROFILE_PASSWORD_MIN_LENGTH 3
#define PROFILE_USERNAME_MIN_LENGTH 3

typedef enum ProfileError ProfileError;
typedef struct Profile Profile;

enum ProfileError {
	PROFILE_ERROR_NONE,									// No error
	PROFILE_ERROR_NO_FILE,							// Profiles file does not exist
	PROFILE_ERROR_NOT_FOUND,						// Logging in into a non-existent profile
	PROFILE_ERROR_ALREADY_EXISTS,				// Attempting to create an existing profile
	PROFILE_ERROR_INVALID_LENGTH,				// Password or username had invalid length
	PROFILE_ERROR_INVALID_CHARS,				// Password or username has invalid chars
	PROFILE_ERROR_INVALID_LOGIN,				// Incorrect password for existing profile
	PROFILE_ERROR_TOO_MANY_EXISTING,		// Too many profiles exist already
};

/**
 * We pass this object to the engine.
*/
struct Profile {
  char sCurrentProfile[PROFILE_USERNAME_MAX_LENGTH + 1];
	ProfileError eError;
};

/**
 * Initializes the profile object.
 * 
 * @param		{ Profile * }		this	The profile object.
*/
void Profile_init(Profile *this) {
  strcpy(this->sCurrentProfile, "");
	this->eError = PROFILE_ERROR_NONE;
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
	int i, j;
	File *pProfilesFile;

	// Store the profiles read from the file
	int nProfileCount = 0;
	char sProfileUsername[PROFILE_USERNAME_MAX_LENGTH + 1];
	char sProfilePassword[PROFILE_PASSWORD_MAX_LENGTH + 1];
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
		sprintf(sProfileUsername, "");
		sprintf(sProfilePassword, "");
		
		// Copy the name first without the newline
		j = 0; 
		while(sProfilesArray[i][j] != ';' && sProfilesArray[i][j])
			sprintf(sProfileUsername, "%s%c", sProfileUsername, sProfilesArray[i][j++]);

		// Copy the password
		j++;
		while(sProfilesArray[i][j] != ';' && sProfilesArray[i][j])
			sprintf(sProfilePassword, "%s%c", sProfilePassword, sProfilesArray[i][j++]);

		// Compare the name
		if(!strcmp(sProfileUsername, sUsername)) {
			
			// Correct login
			if(!strcmp(sProfilePassword, sPassword)) {
				i = nProfileCount + 1;

			// Invalid login
			} else {
				this->eError = PROFILE_ERROR_INVALID_LOGIN;
				return 0;
			}
		}
	}

	// Profile waasn't found
	if(i == nProfileCount) {
		this->eError = PROFILE_ERROR_NOT_FOUND;
		return 0;
	}

	// Set the current profile to the logged in username
	strcpy(this->sCurrentProfile, sUsername);

	// Garbage collection
	File_kill(pProfilesFile);
	
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
	
	int nProfileCount = 0;
	char sProfileEntry[PROFILE_USERNAME_MAX_LENGTH + 1];
	char *sNewProfile[1];
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
		this->eError = PROFILE_ERROR_TOO_MANY_EXISTING;
		String_kill(sNewProfile[0]);
		
		return 0;
	}

	// The profile already exists
	for(i = 0; i < nProfileCount; i++) {
		strcpy(sProfileEntry, "");

		// Copy the name first without the newline
		j = 0;
		while(sProfilesArray[i][j] != ';' && sProfilesArray[i][j])
			sprintf(sProfileEntry, "%s%c", sProfileEntry, sProfilesArray[i][j++]);
		
		// Compare the name
		if(!strcmp(sProfileEntry, sUsername)) {
			File_kill(pProfilesFile);

			this->eError = PROFILE_ERROR_ALREADY_EXISTS;
			return 0;
		}
	}

	// Otherwise, append the profile to the file
	strcat(sNewProfile[0], ";");
	strcat(sNewProfile[0], sPassword);
	strcat(sNewProfile[0], ";\n");
	File_writeText(pProfilesFile, 1, sNewProfile);

	// Garbage collection
	File_kill(pProfilesFile);
	String_kill(sNewProfile[0]);

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

	// Store the profiles read from the file
	int nProfileCount = 0;
	char sProfileUsername[PROFILE_USERNAME_MAX_LENGTH + 1];
	char *sProfileEntry[1];
	char *sProfilesArray[PROFILES_MAX_NUM];

	// No data to cross check login
	if(!File_exists(PROFILES_FILE_PATH)) {
		this->eError = PROFILE_ERROR_NO_FILE;
		return 0;
	}

	// Open the file
	pProfilesFile = File_create(PROFILES_FILE_PATH);

	// Read the text onto the output buffer
	File_readText(pProfilesFile, PROFILES_MAX_NUM, &nProfileCount, sProfilesArray);

	// Clear the file
	File_clear(pProfilesFile);

	// The profile exists
	for(i = 0; i < nProfileCount; i++) {
		sprintf(sProfileUsername, "");
		sProfileEntry[0] = String_create(sProfilesArray[i]);
		
		// Copy the name first without the newline
		j = 0; 
		while(sProfilesArray[i][j] != ';' && sProfilesArray[i][j])
			sprintf(sProfileUsername, "%s%c", sProfileUsername, sProfilesArray[i][j++]);

		// Copy to file only if not the selected profile
		if(strcmp(sProfileUsername, sUsername))
			File_writeText(pProfilesFile, 1, sProfileEntry);

		// Kill the allocated string
		String_kill(sProfileEntry[0]);
	}

	// ! TODO DESTROY THE PROFILE FILE ASSOCIATED WITH THE USER

	// Set the current profile to an empty string
	strcpy(this->sCurrentProfile, "");

	// Garbage collection
	File_kill(pProfilesFile);
	
	// Successful
	return 1;
}

/**
 * Saves the data newly-created profile in its corresponding text file.
 * 
 * //!! FIX THISS
 * 
 * @param   { char * }      sName       Name of the profile.
*/
void Profile_save(char *sName) {
    int i;

    // Path of the profile's text file
    char *sPath = String_alloc(PROFILE_FILE_PATH_MAX_LENGTH);    

    // Completes the file path of the profile's text file
    snprintf(sPath, PROFILE_FILE_PATH_MAX_SIZE, "%s%s.txt", PROFILE_FOLDER_PATH, sName);

    // Creates and opens the profile's text file
    FILE *pProfile = fopen(sPath, "w");

    // Prints the profile's name on the file
    fprintf(pProfile, "%s\n", sName);

    // Prints the initial number of games won for each game type
    // Respective game types: Classic - Easy, Classic - Difficult, Custom
    fprintf(pProfile, "0 0 0\n");

    // Prints the initial game data of the three most recent games
    for(i = 0; i < 3; i++) {

        // Prints out the game's initial type, difficulty, outcome, and
        //    the field's width and height
        fprintf(pProfile, "-1 -1 -1 1 1\n");

        // Prints out the game's initial field
        fprintf(pProfile, ".\n");
    }

    // Deallocates the memory of the name's string
    String_kill(sPath);

    fclose(pProfile);
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

		default:
			return "Error: no error.";
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