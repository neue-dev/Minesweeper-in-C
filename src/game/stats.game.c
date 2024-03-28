/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 17:01:04
 * @ Modified time: 2024-03-28 22:12:41
 * @ Description:
 * 
 * Displays the statistics of a profile.
 */

#ifndef STATS_
#define STATS_

#include "profile.game.c"

#include "../utils/utils.string.h"

#include <stdio.h>

/**
 * Copies the profile's stats from its text file to its object.
 * 
 * @param   { char * }      sName       Name of the profile.
 * @param   { Profile * }   pProfile    A pointer to the profile's object.
*/
Stats_updateObject(char *sName, Profile *pProfile) {
    
    // Path of the profile's text file
    char *sPath = String_alloc(PROFILE_FILE_PATH_MAX_LENGTH);    

    // Completes the file path of the profile's text file
    snprintf(sPath, PROFILE_FILE_PATH_MAX_SIZE, "%s%s.txt", PROFILE_FOLDER_PATH, sName);

    // Opens the profile's text file
    FILE *pProfile = fopen(sPath, "r");

    if(pProfile == NULL)
        return; // TODO: error-handling

    // Acquires the profile's name
    fscanf("%s ", pProfile->sName);

    // Acquires the profile's number of wins
    fscanf("%d %d %d ", pProfile->nClassicEasy, pProfile->nClassicDifficult,
                        pProfile->nCustom);

    // TODO: Acquire the profile's most recent game data

    fclose(pProfile);

    // Deallocates the memory of the path's string
    String_kill(sPath);
}

/**
 * Copies the profile's stats from its object to its text file.
 * 
 * @param   { Profile * }   pProfile    A pointer to the profile's object.
*/
void Stats_updateFile(Profile *pProfile) {

    // Path of the profile's text file
    char *sPath = String_alloc(PROFILE_FILE_PATH_MAX_LENGTH);    

    // Completes the file path of the profile's text file
    snprintf(sPath, PROFILE_FILE_PATH_MAX_SIZE, "%s%s.txt", 
                    PROFILE_FOLDER_PATH, pProfile->sName);

    // Opens the profile's text file
    FILE *pProfile = fopen(sPath, "r");

    if(pProfile == NULL)
        return; // TODO: error-handling

    // Prints out the profile's name
    fprintf("%s\n", pProfile->sName);

    // Prints out the profile's number of wins per game type
    fprintf("%d %d %d\n", pProfile->nClassicEasy, pProfile->nClassicDifficult,
                          pProfile->nCustom);
    
    // TODO: Print out the profile's most recent game data

    fclose(pProfile);

    // Deallocates the memory of the path's string
    String_kill(sPath);
}

/**
 * Updates the data for the three most recent games.
 * Operates when a new game has recently ended.
*/
void Stats_addGame() {
    // TODO: add functionalities
}

#endif