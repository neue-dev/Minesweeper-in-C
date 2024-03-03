/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-03-03 20:51:08
 * @ Description:
 * 
 * This file contains definitions for animation handlers (basically, functions that increment 
 *    the animation over time).
 * Note that these functions are passed as callbacks to animation instances.
 */

#ifndef ANIMATIONS_
#define ANIMATIONS_

#include "./utils/utils.asset.h"
#include "./utils/utils.animation.h"
#include "./utils/utils.buffer.h"
#include "./utils/utils.page.h"
#include "./utils/utils.types.h"

#include <math.h>
#include <stdlib.h>

/**
 * //
 * ////
 * //////    Animation handlers
 * ////////
 * ////////// 
*/

/**
 * Handles the intro animations.
 * 
 * @param   { p_obj }   Args_Animation  The animation object that contains our state data.
 * @param   { p_obj }   Args2_Page      The page object we're gonna update based on our state data.
*/
void AnimationHandler_intro(p_obj Args_Animation, p_obj Args2_Page) {
  Animation *this = (Animation *) Args_Animation;
  Page *pPage = (Page *) Args2_Page;
  Buffer *pBuffer = pPage->pPageBuffer;
 
  int dWidth = pBuffer->dWidth;
  int dHeight = pBuffer->dHeight;
  int i, dCumulativeLen = 0;

  char **aTitleGlyph;                     // We prepend asset variable names with a.
  char sTitleGlyphs[12] = "minesweeper";  // A list of the characters we need from the asset set.
  char *sGlyphName = String_alloc(16);    // We'll use this to get the specific name of the asset.

  // Initialization stage
  if(this->eAnimationState == ANIMATION_INIT) {

    // Init the states we need; these refer to the y positions of each letter
    for(i = 0; i < 11; i++) {
      this->dStates[i * 2] = 1;
      this->fStates[i * 2] = 0;   // Dummy values so theyre not null to begin with

      this->dStates[i * 2 + 1] = 10;
      this->fStates[i * 2 + 1] = i * i * i * 10.0 - 128.0;
    }

    // After the text centered, we expand the boundary
    this->dStates[22] = -1;
    this->fStates[22] = 5.0;

    // You have to manually set these cuz u didnt pass a statecount in the constuctor
    this->dFloatStateCount = 23;
    this->dIntStateCount = 23;

  // Running the animation
  } else {
    
    // Update the position of the title along the x axis
    for(i = 0; i < 11; i++) {
      this->dStates[i * 2] = dCumulativeLen;
      
      // Get the name of the asset
      sprintf(sGlyphName, "main-font-%c", sTitleGlyphs[i]);
      aTitleGlyph = AssetManager_getAssetText(pPage->pSharedAssetManager, sGlyphName);

      // Compute these
      dCumulativeLen += String_length(aTitleGlyph[0]);
    }

    // Shift towards the center
    for(i = 0; i < 11; i++)
      this->dStates[i * 2] += dWidth / 2 - dCumulativeLen / 2 - 1;

    // Perform different updates based on the stage of the animation
    switch(this->dStage) {
      
      case 0:   // Make the letters approach the center

        // Make the fStates approach their targets (this is along the y-axis)
        for(i = 0; i < 11; i++)
          this->fStates[i * 2 + 1] = Math_easeOut(this->fStates[i * 2 + 1], this->dStates[i * 2 + 1], 0.75);

        // Go to next stage
        if(Math_dist1d(this->fStates[1], this->dStates[1] * 1.0) < 0.001) {          
          for(i = 0; i < 11; i++)
            this->dStates[i * 2 + 1] = -10;
          this->dStage++;
        }
      break;

      case 1:   // Transition into the menu page

        // Update the letter y positions
        for(i = 0; i < 11; i++)
          this->fStates[i * 2 + 1] = Math_easeIn(this->fStates[i * 2 + 1], this->dStates[i * 2 + 1], 0.75);
        
        // Update the bounding box
        this->fStates[22] = Math_easeIn(this->fStates[22], this->dStates[22], 0.83);

        // Go to next stage
        if(Math_dist1d(this->fStates[1], this->dStates[1] * 1.0) < 0.001)
          this->dStage++;
      break;

      default:
        this->eAnimationState = ANIMATION_EXIT;
        pPage->ePageState = PAGE_ACTIVE_IDLE;
      break;
    }
    
    // Make sure we can display signature at the bottom
    Buffer_contextRect(pBuffer, 0, 0, dWidth, dHeight, 0xffffff, 0x000000);

    // Total black at the edges
    Buffer_contextRect(pBuffer, 0, 0, dWidth - 1, dHeight, 0x000000, 0x000000);

    // A red bounding box
    Buffer_contextRect(pBuffer, 
      this->dRoundStates[22] * 2, 
      this->dRoundStates[22], 
      
      dWidth - this->dRoundStates[22] * 4, 
      dHeight - this->dRoundStates[22] * 2, 
      0xcc2121, 0xcc2121);

    // White canvas in center
    Buffer_contextRect(pBuffer, 
      this->dRoundStates[22] * 2 + 2, 
      this->dRoundStates[22] + 1, 
      
      dWidth - this->dRoundStates[22] * 4 - 4, 
      dHeight - this->dRoundStates[22] * 2 - 2, 
      0x111111, 0xffffff);

    // This displays the intro title
    for(i = 0; i < 11; i++) {

      // Generate the key to get the character asset
      sprintf(sGlyphName, "main-font-%c", sTitleGlyphs[i]);

      // Write the asset to the buffer
      Buffer_write(pBuffer, 
        this->dStates[i * 2], 
        this->dRoundStates[i * 2 + 1], 

        AssetManager_getAssetHeight(pPage->pSharedAssetManager, sGlyphName),
        AssetManager_getAssetText(pPage->pSharedAssetManager, sGlyphName));
    }
  }
}

/**
 * Handles the menu animations.
 * 
 * @param   { p_obj }   Args_Animation  The animation object that contains our state data.
 * @param   { p_obj }   Args2_Page      The page object we're gonna update based on our state data.
*/
void AnimationHandler_menu(p_obj Args_Animation, p_obj Args2_Page) {
  Animation *this = (Animation *) Args_Animation;
  Page *pPage = (Page *) Args2_Page;
  Buffer *pBuffer = pPage->pPageBuffer;
 
  int dWidth = pBuffer->dWidth;
  int dHeight = pBuffer->dHeight;

  char **aLogo;   // The logo for the game
  char **aTitle;  // The title of the game

  // We perform the initialization here
  if(this->eAnimationState == ANIMATION_INIT) {

    // The bomb logo
    this->dStates[0] = 3;
    this->fStates[0] = 100.0;

    // The title
    this->dStates[1] = 18;
    this->fStates[1] = 100.0;

    this->dIntStateCount = 2;
    this->dFloatStateCount = 2;

  // This is where the animation updates happen
  } else {

    // Perform different updates based on the stage of the animation
    switch(this->dStage) {

      case 3:   // Transition to next page
        this->dStates[0] = -35;
        this->dStates[1] = -20;

        this->fStates[0] = Math_easeIn(this->fStates[0], this->dStates[0], 0.93);
        this->fStates[1] = Math_easeIn(this->fStates[1], this->dStates[1], 0.93);
      break;
      
      case 2:   // We just chillin
        if(EventStore_get(pPage->pSharedEventStore, "key-pressed") == 't')
          this->dStage++;

      case 1:   // Make the title go up
        this->fStates[1] = Math_easeOut(this->fStates[1], this->dStates[1], 0.75);

        if(Math_dist1d(this->fStates[1], this->dStates[1] * 1.0) < 10 && this->dStage == 1)
          this->dStage++;

      case 0:   // Make the bomb go up
        this->fStates[0] = Math_easeOut(this->fStates[0], this->dStates[0], 0.75);

        if(Math_dist1d(this->fStates[0], this->dStates[0] * 1.0) < 10 && this->dStage == 0)
          this->dStage++;
      break;

      default:
        this->eAnimationState = ANIMATION_EXIT;
        pPage->ePageState = PAGE_ACTIVE_IDLE;
      break;
    }

    // Background
    Buffer_contextRect(pBuffer, 0, 0, dWidth, dHeight, 0x111111, 0xffffff);
    
    // The minesweeper logo and title
    aLogo = AssetManager_getAssetText(pPage->pSharedAssetManager, "logo");
    aTitle = AssetManager_getAssetText(pPage->pSharedAssetManager, "title");

    // Makes the title a different color
    Buffer_contextRect(pBuffer, 
      dWidth / 2 - String_length(aTitle[0]) / 2 - 1, 
      this->dRoundStates[1], 
      strlen(aTitle[0]), 1, 
      0x000000, 0xffffff);

    Buffer_write(pBuffer, 
      dWidth / 2 - String_length(aLogo[0]) / 2 - 1, 
      this->dRoundStates[0], 
      AssetManager_getAssetHeight(pPage->pSharedAssetManager, "logo"),
      aLogo);

    Buffer_write(pBuffer, 
      dWidth / 2 - String_length(aTitle[0]) / 2 - 1, 
      this->dRoundStates[1], 
      AssetManager_getAssetHeight(pPage->pSharedAssetManager, "title"),
      aTitle);
  }
}

#endif
