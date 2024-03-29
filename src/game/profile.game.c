/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-27 2:13:51
 * @ Modified time: 2024-03-29 20:46:22
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
//! REMOVE
#include "../utils/utils.debug.h"
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
	File_readText(pProfilesFile, PROFILES_MAX_NUM, &nProfileCount, sProfilesArray);

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
int Profile_create(Profile *this, char *sUsername, char *sPassword) {
	int i, j;
	File *pProfilesFile;
	
	int nProfileCount = 0;
	char sProfileEntry[PROFILE_USERNAME_MAX_LENGTH + 1];
	char *sNewProfile[1];
	char *sProfilesArray[PROFILES_MAX_NUM];

	// Name was of invalid size
	if(strlen(sUsername) < PROFILE_USERNAME_MIN_LENGTH || strlen(sUsername) > PROFILE_USERNAME_MAX_LENGTH) {
		this->eError = PROFILE_ERROR_INVALID_LENGTH;
		return 0;
	}

	// Password was too long
	if(strlen(sPassword) < PROFILE_PASSWORD_MIN_LENGTH || strlen(sPassword) > PROFILE_PASSWORD_MAX_LENGTH)

	// Checks if the name's characters are only capital letters
	i = 0;
	do {
		if(sUsername[i] < 'A' || sUsername[i] > 'Z') {
			this->eError = PROFILE_ERROR_INVALID_CHARS;
			return 0;
		}
	} while(sUsername[++i]);

	// Set the current profile to the given name
	sNewProfile[0] = String_alloc(strlen(sUsername) + strlen(sPassword) + 3);
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
	File_readText(pProfilesFile, PROFILES_MAX_NUM, &nProfileCount, sProfilesArray);

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
	strcat(sNewProfile[0], ";");
	File_writeText(pProfilesFile, 1, sNewProfile);

	// Garbage collection
	File_kill(pProfilesFile);
	String_kill(sNewProfile[0]);

	// Successful
	return 1;
}

/**
 * Selects a profile.
 * 
 * @param   { char * }  sKey   Name of the to-be-selected profile.
*/
void Profile_select(char *sKey) {
    int i;
    int nProfiles;
    int bProfileFound = 0;
    char *sCurrentProfile = String_alloc(PROFILE_USERNAME_MAX_LENGTH);

    // Opens the text file with the list of profiles
    FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // An array of the profile names
    char aProfiles[PROFILES_MAX_NUM][PROFILE_USERNAME_MAX_LENGTH + 1] = {{}};

    // Gets the previously-selected profile name
    fscanf(pProfiles, "%s ", sCurrentProfile);

    // Gets the number of existing profiles
    fscanf(pProfiles, "%d ", nProfiles);

    i = 0;
    while(!feof(pProfiles)) {

        // Gets the profile name.
        // Note that this initially gets the number of profiles that
        // exist (first line of the text file).
        fscanf(pProfiles, "%s ", aProfiles[i]);

        // The profile has been found
        if(strcmp(aProfiles[i], sKey) == 0) {
            bProfileFound = 1;

            // Updates the currently-selected profile
            strcpy(sCurrentProfile, sKey);
        }

        i++;
    }

    fclose(pProfiles);

    // We will now write the new data onto the file
    pProfiles = fopen(PROFILES_FILE_PATH, "w");

    // Prints out the currently-selected profile
    fprintf(pProfiles, "%s\n", sCurrentProfile);

    // Prints out the number of existing profiles
    fprintf(pProfiles, "%d\n", nProfiles);

    // Prints out the list of existing profile names
    for(i = 0; i < nProfiles; i++) {
        fprintf(pProfiles, "%s\n", aProfiles[i]);
    }

    // Deallocates the memory of the current profile name's string
    String_kill(sCurrentProfile);

    fclose(pProfiles);
    
    // ! todo
    if(!bProfileFound) {}
    // Profile_doesNotExist();
}

/**
 * Deletes a profile.
 * 
 * @param   { char * }  sName   Name of the to-be-selected profile.
*/
void Profile_delete(char *sName) {
    int i;
    int nProfiles;
    int dProfileIndex = -1;
    char *sCurrentProfile = String_alloc(PROFILE_USERNAME_MAX_LENGTH);

    // Opens the text file with the list of profiles
    FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // An array of the profiles
    char aProfiles[PROFILES_MAX_NUM][PROFILE_USERNAME_MAX_LENGTH + 2] = {{}};

    // Gets the number of profiles that exist
    fscanf(pProfiles, "%d ", &nProfiles);

    // Gets the currently-selected profile name
    fscanf(pProfiles, "%s ", sCurrentProfile);

    // If no profile can be deleted
    if(nProfiles == 0) {
        fclose(pProfiles);

        // Deallocates the memory of the currently-selected profile name
        String_kill(sCurrentProfile);
        
        // ! todo
        // Profile_doesNotExist();
        return; // Exits the function
    }
        
    i = 0;
    while(!feof(pProfiles)) {

        // Stores the list of profiles to an array
        fscanf(pProfiles, "%s ", aProfiles[i]);

        // Specifies the index of the found profile
        if(strcmp(aProfiles[i], sName) == 0)
            dProfileIndex = i;

        i++;
    }

    // Checks if the profile has been found
    if(dProfileIndex == -1) {
        fclose(pProfiles);

        // Deallocates the memory of the currently-selected profile name
        String_kill(sCurrentProfile);
        
        // ! todo
        // Profile_doesNotExist();
        return; // Exits the function
    }

    fclose(pProfiles);

    // We will now write the new data to the text file of profiles
    pProfiles = fopen(PROFILES_FILE_PATH, "w");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // Updates the number of profiles that exist
    nProfiles--;
    fprintf(pProfiles, "%d\n", nProfiles);

    // Prints out the currently-selected profile
    fprintf(pProfiles, "%s\n", sCurrentProfile);

    // Prints out the undeleted profile names
    for(i = 0; i <= nProfiles; i++) {
        if(i != dProfileIndex)
            fprintf(pProfiles, "%s\n", aProfiles[i]);
    }

    // Deallocates the memory of the current profile name's string
    String_kill(sCurrentProfile);

    fclose(pProfiles);
}

/**
 * Saves the data newly-created profile in its corresponding text file.
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

		case PROFILE_ERROR_NONE:
			return "Error: no error.";
	}
}

/**
 * An error-handling function.
 * Operates when the number of existing profiles exceed the max (10).
 * 
*/
void Profile_maxNumReached() {
    // TODO: Code this function considering the GUI.
    //       Display the error message.
}

/**
 * An error-handling function.
 * Operates when the user inputs an invalid profile name.
 * 
*/
void Profile_invalidName() {
    // TODO: Code this function considering the GUI.
    //       Display the error message.
}

/**
 * An error-handling function.
 * Operates when the name of the to-be-created profile already exists.
 * 
*/
void Profile_exists() {
    // TODO: Code this function considering the GUI.
    //       Display the error message.
}

/**
 * An error-handling function.
 * Operates when the name of the to-be-selected/deleted file does not exist.
 * 
*/
void Profile_doesNotExist() {
    // TODO: Code this function considering the GUI.
    //       Display the error message.
}

#endif