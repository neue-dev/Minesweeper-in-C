/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-28 17:01:04
 * @ Modified time: 2024-03-29 14:44:55
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
 * Saves the data of a game that had recently ended.
 * 
 * @param   { Game * }      pGame       The game object.
*/
void Stats_saveGame(Game *pGame) {
    int i, j;

    // get profile name

    // store main data + 2 games
    
    // print main data
    // print new game
    // print 2 games
    
    // Opens the game's text file
    FILE *pGame = fopen(GAME_FILE_PATH, "a");

    // ! NEW GAME

    // Prints out the game data
    fprintf("%d %d %d %d %d\n", pGame->eType, pGame->eDifficulty, pGame->eOutcome,
                                pGame->gameField.dWidth, pGame->gameField.dHeight);

    // Prints out the game's field
    for(i = 0; i < pGame->gameField.dHeight; i++) {
        for(j = 0; j < pGame->gameField.dWidth; j++) {

            // Prints out the flags
            // Outcome: quit/loss
            if(pGame->eOutcome != GAME_OUTCOME_WIN &&
               Grid_getBit(pGame->gameField.pFlagGrid, j, i))
                fprintf(pGame, "F");
            
            // Prints out the mines
            else if(pGame->gameField.aNumbers[i][j] < 0) {

                // Outcome: quit; Does not show any mine
                if(pGame->eOutcome == GAME_OUTCOME_QUIT)
                    fprintf(pGame, "?");

                // Outcome: loss; Shows the mine that exploded
                else if(pGame->gameField.aNumbers[i][j] == -2)
                    fprintf(pGame, "H");

                // Outcome: win/loss; Shows all unexploded mines
                else
                    fprintf(pGame, "X");

            // Prints out the tile numbers
            } else {

                // Outcome: win; Shows all numbers
                // Outcome: quit/loss; Shows numbers that were revealed
                if(pGame->eOutcome == GAME_OUTCOME_WIN ||
                   Grid_getBit(pGame->gameField.pInspectGrid, j, i))
                    fprintf(pGame, "%d", pGame->gameField.aNumbers[i][j]);
                
                // Outcome: quit/loss; Does not show numbers that were unrevealed
                else
                    fprintf(pGame, "?");
            }

            // Prints out a newline after every row, and space otherwise
            fprintf(pGame, (j == pGame->gameField.dWidth - 1) ? "\n" : " ");
        }
    }

    fclose(pGame);
}

#endif