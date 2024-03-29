/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-27 2:13:51
 * @ Modified time: 2024-03-29 16:52:50
 * @ Description:
 * 
 * Executes tasks involved in-game.
 */

#ifndef PROFILE_
#define PROFILE_

#include "game.c"

#include "../utils/utils.string.h"

#include <stdio.h>
#include <string.h>

#define PROFILES_FILE_PATH "../data/profiles.data.txt"
#define GAME_FILE_PATH  "../data/game.data.txt"

#define PROFILES_MAX_NUM 10

#define PROFILE_FOLDER_PATH "../data/profiles/"
#define PROFILE_FOLDER_PATH_LENGTH strlen(PROFILE_FOLDER_PATH)

// +4 includes ".txt"
#define PROFILE_FILE_PATH_MAX_LENGTH (PROFILE_FOLDER_PATH_LENGTH + PROFILE_NAME_MAX_LENGTH + 4)
#define PROFILE_FILE_PATH_MAX_SIZE sizeof(char)*(PROFILE_FILE_PATH_MAX_LENGTH + 1)

#define PROFILE_NAME_MAX_LENGTH 20
#define PROFILE_NAME_MIN_LENGTH 3

#define PROFILE_NAME_MAX_SIZE sizeof(char)*(PROFILE_NAME_MAX_LENGTH + 1) 
#define PROFILE_NAME_MIN_SIZE sizeof(char)*(PROFILE_NAME_MIN_LENGTH + 1)

/**
 * Creates a new profile.
 * 
 * @param   { char * }  sName   Name of the to-be-created profile.
*/
void Profile_create(char *sName) {
    int i;
    int nProfiles = 0;
    int bNameAdded = 0;
    char *sCurrentProfile = String_alloc(PROFILE_NAME_MAX_LENGTH);

    // Opens the text file with the list of profiles
    FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // An array of the profile names
    char aProfiles[PROFILES_MAX_NUM][PROFILE_NAME_MAX_LENGTH + 2] = {{}};

    // Checks if the name has the right number of characters (3 to 20)
    if(strlen(sName) < 3 || strlen(sName) > 20) {
        fclose(pProfiles);

        // Deallocates the memory of the currently-selected profile name
        String_kill(sCurrentProfile);
        
        // ! todo
        // Profile_invalidName();
        return; // Exits the function
    }

    // Gets the name of the currently-selected profile
    fscanf(pProfiles, "%s ", sCurrentProfile);

    // Gets the number of profiles that exist
    fscanf(pProfiles, "%d ", &nProfiles);

    // Checks if the number of profiles exceed the max
    if(nProfiles == PROFILES_MAX_NUM) {
        fclose(pProfiles);

        // Deallocates the memory of the currently-selected profile name
        String_kill(sCurrentProfile);
        
        // ! todo
        // Profile_maxNumReached();
        return; // Exits the function
    }

    // Checks if the name's characters are only capital letters
    i = 0;
    while(*(sName + i) != '\0') {
        if(*(sName + i) < 'A' || *(sName + i) > 'Z') {
            fclose(pProfiles);

            // Deallocates the memory of the currently-selected profile name
            String_kill(sCurrentProfile);

            // ! todo
            // Profile_invalidName();
            return; // Exits the function
        }
        i++;
    }

    // Adds the name to the array if no profiles exist yet
    if(nProfiles == 0)
        strcpy(aProfiles[0], sName);
    
    i = 0;
    while(!feof(pProfiles) && nProfiles) {

        // Stores the list of profiles to an array
        fscanf(pProfiles, "%s ", aProfiles[i]);

        // Checks if the name of the to-be-created profile already exists
        if(!bNameAdded && strcmp(sName, aProfiles[i]) == 0) {
            fclose(pProfiles);

            // Deallocates the memory of the currently-selected profile name
            String_kill(sCurrentProfile);

            // ! todo
            // Profile_exists();
            return; // Exits the function
        }

        // Adds the name to the array if it has not yet been added and
        // the end of profile list has been reached
        if(!bNameAdded && (i == nProfiles - 1 || nProfiles == 0)) {
            strcpy(aProfiles[i+1], sName);
            bNameAdded = 1;
        }

        // Adds the name to the array if it has not yet been added and
        // its ASCII value is lesser than the currently added profile name
        // Ensures that the name is added in accordance with the alphabetical order
        if(!bNameAdded && strcmp(sName, aProfiles[i]) < 0) {

            // Moves the currently added profile name to the next address of the array
            strcpy(aProfiles[i+1], aProfiles[i]);

            // Adds the name to the current array address
            strcpy(aProfiles[i], sName);

            // Increments i since we've already dealt with the next array address
            i++;

            bNameAdded = 1;
        }
        
        i++;
    }

    fclose(pProfiles);

    // We will now write the new data to the text file of profiles
    pProfiles = fopen(PROFILES_FILE_PATH, "w");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // Prints out the currently-selected profile
    fprintf(pProfiles, "%s\n", sCurrentProfile);

    // Updates the number of profiles that exist
    nProfiles++;
    fprintf(pProfiles, "%d\n", nProfiles);

    // Prints each of the names in the array to the file
    for(i = 0; i < nProfiles; i++) {
        fprintf(pProfiles, "%s\n", aProfiles[i]);
    }

    // Deallocates the memory of the currently-selected profile name
    String_kill(sCurrentProfile);

    fclose(pProfiles);
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
    char *sCurrentProfile = String_alloc(PROFILE_NAME_MAX_LENGTH);

    // Opens the text file with the list of profiles
    FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // An array of the profile names
    char aProfiles[PROFILES_MAX_NUM][PROFILE_NAME_MAX_LENGTH + 1] = {{}};

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
    char *sCurrentProfile = String_alloc(PROFILE_NAME_MAX_LENGTH);

    // Opens the text file with the list of profiles
    FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // An array of the profiles
    char aProfiles[PROFILES_MAX_NUM][PROFILE_NAME_MAX_LENGTH + 2] = {{}};

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