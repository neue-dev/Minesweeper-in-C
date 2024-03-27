/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-27 2:13:51
 * @ Modified time: 2024-03-27 20:54:55
 * @ Description:
 * 
 * Executes tasks involved in-game.
 */

#include "../utils/utils.string.h"

#include <stdio.h>
#include <string.h>

#define PROFILES_FILE_PATH "../data/profiles.data.txt"

#define PROFILES_MAX_NUM 10

#define PROFILE_NAME_MAX_LENGTH 20
#define PROFILE_NAME_MIN_LENGTH 3

#define PROFILE_NAME_MAX_SIZE sizeof(char)*(PROFILE_NAME_MAX_LENGTH + 2) // +2 includes \0 and \n
#define PROFILE_NAME_MIN_SIZE sizeof(char)*(PROFILE_NAME_MIN_LENGTH + 2)

/**
 * Creates a new profile.
 * 
 * @param   { char * }  sName   Name of the to-be-created profile.
*/
void Profile_create(char *sName) {
    int i;
    int nProfiles = 0;
    int bNameAdded = 0;

    // Opens the text file with the list of profiles
    FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // An array of the profiles
    char aProfiles[PROFILES_MAX_NUM][PROFILE_NAME_MAX_LENGTH + 2] = {{}};

    // Checks if the name has the right number of characters (3 to 20)
    if(strlen(sName) < 3 || strlen(sName) > 20) {
        Profile_invalidName();
        return; // Exits the function
    }

    // Gets the number of profiles that exist
    fscanf(pProfiles, "%d ", &nProfiles);

    // Checks if the number of profiles exceed the max
    if(nProfiles == PROFILES_MAX_NUM) {
        Profile_maxNumReached();
        return; // Exits the function
    }

    // Checks if the name's characters are only capital letters
    i = 0;
    while(*(sName + i) != '\0') {
        if(*(sName + i) < 'A' || *(sName + i) > 'Z') {
            Profile_invalidName();
            return; // Exits the function
        }
        i++;
    }

    // Concatinates the key's string with \n, considering that fgets does this.
    // This is to prevent runtime errors with strcmp().
    strcat(sName, "\n");

    // Adds the name to the array if no profiles exist yet
    if(nProfiles == 0)
        strcpy(aProfiles[0], sName);
    
    i = 0;
    while(!feof(pProfiles) && nProfiles) {

        // Stores the list of profiles to an array
        fgets(aProfiles[i], PROFILE_NAME_MAX_SIZE, pProfiles);

        // Checks if the name of the to-be-created profile already exists
        if(!bNameAdded && strcmp(sName, aProfiles[i]) == 0) {
            Profile_exists();
            return; // Exits the function
        }

        // Adds the name to the array if it has not yet been added and
        // the end of profile list has been reached
        if(!bNameAdded && (i == nProfiles - 1 || nProfiles == 0)) {
            strcat(aProfiles[i], "\n");
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

    // Updates the number of profiles that exist
    nProfiles++;
    fprintf(pProfiles, "%d\n", nProfiles);

    // Prints each of the names in the array to the file
    for(i = 0; i < nProfiles; i++) {
        fprintf(pProfiles, "%s", aProfiles[i]);
    }

    fclose(pProfiles);
}

/**
 * Selects a profile.
 * 
 * @param   { char * }  sKey   Name of the to-be-selected profile.
*/
void Profile_select(char *sKey) {
    int i;
    char *sName = String_alloc(PROFILE_NAME_MAX_LENGTH + 1); // +1 includes \n

    // Opens the text file with the list of profiles
    FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // Concatinates the key's string with \n, considering that fgets does this.
    // This is to prevent runtime errors with strcmp().
    strcat(sKey, "\n");

    i = 0;
    while(!feof(pProfiles)) {

        // Gets the profile name.
        // Note that this initially gets the number of profiles that
        // exist (first line of the text file).
        fgets(sName, PROFILE_NAME_MAX_SIZE, pProfiles);

        // Terminates the function once the profile has been found
        if(strcmp(sName, sKey) == 0)
            return;

        i++;
    }

    fclose(pProfiles);

    String_kill(sName);

    Profile_doesNotExist();
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

    // Opens the text file with the list of profiles
    FILE *pProfiles = fopen(PROFILES_FILE_PATH, "r");

    if(pProfiles == NULL)
        return; // TODO: error-handling

    // An array of the profiles
    char aProfiles[PROFILES_MAX_NUM][PROFILE_NAME_MAX_LENGTH + 2] = {{}};

    // Gets the number of profiles that exist
    fscanf(pProfiles, "%d ", &nProfiles);

    // If no profile can be deleted
    if(nProfiles == 0) {
        Profile_doesNotExist();
        return; // Exits the function
    }
        

    // Concatinates the key's string with \n, considering that fgets does this.
    // This is to prevent runtime errors with strcmp().
    strcat(sName, "\n");

    i = 0;
    while(!feof(pProfiles)) {

        // Stores the list of profiles to an array
        fgets(aProfiles[i], PROFILE_NAME_MAX_SIZE, pProfiles);

        // Specifies the index of the found profile
        if(strcmp(aProfiles[i], sName) == 0)
            dProfileIndex = i;

        i++;
    }

    // Checks if the profile has been found
    if(dProfileIndex == -1) {
        Profile_doesNotExist();
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

    // Prints each of the names in the array to the file except the deleted one
    for(i = 0; i <= nProfiles; i++) {
        if(i != dProfileIndex)
            fprintf(pProfiles, "%s", aProfiles[i]);
    }

    fclose(pProfiles);
}

/**
 * An error-handling function.
 * Operates when the number of existing profiles exceed the max (10).
 * 
*/
void Profile_maxNumReached() {
    // TODO: Code this function considering the GUI.
}

/**
 * An error-handling function.
 * Operates when the user inputs an invalid profile name.
 * 
*/
void Profile_invalidName() {
    // TODO: Code this function considering the GUI.
}

/**
 * An error-handling function.
 * Operates when the name of the to-be-created profile already exists.
 * 
*/
void Profile_exists() {
    // TODO: Code this function considering the GUI.
}

/**
 * An error-handling function.
 * Operates when the name of the to-be-selected/deleted file does not exist.
 * 
*/
void Profile_doesNotExist() {
    // TODO: Code this function considering the GUI.
}
