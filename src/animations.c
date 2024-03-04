/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-03-04 11:59:52
 * @ Description:
 * 
 * This file contains definitions for animation handlers (basically, functions that increment 
 *    the animation over time).
 * Note that these functions are passed as callbacks to animation instances.
 * Functions here are also annotated differently for convenience.
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
  
  /**
   * Grab all the pertinent objects and store some constant values.
   * ..............................................................
  */
  Animation *this = (Animation *) Args_Animation;
  Page *pPage = (Page *) Args2_Page;
  Buffer *pBuffer = pPage->pPageBuffer;
 
  int dWidth = pBuffer->dWidth;
  int dHeight = pBuffer->dHeight;
  
  int dRectPos;
  char **aLogo = AssetManager_getAssetText(pPage->pSharedAssetManager, "logo");
  int dLogoHeight = AssetManager_getAssetHeight(pPage->pSharedAssetManager, "logo");

  /**
   * Initialization stage.
   * .....................
  */
  if(this->eAnimationState == ANIMATION_INIT) {

    // The bomb logo
    this->dStates[0] = round(dHeight / 2.0 - dLogoHeight / 2.0) - 1;
    this->fStates[0] = 100.0;

    // The bounding box
    this->dStates[1] = 5;
    this->fStates[1] = dWidth / 2;

    // You have to manually set these cuz u didnt pass a statecount in the constuctor
    this->dFloatStateCount = 2;
    this->dIntStateCount = 2;

  /**
   * Update stage.
   * .............
  */
  } else {

    // Perform different updates based on the stage of the animation
    switch(this->dStage) {
      
      case 0:   // Create the boundary area first

        // Make the size approach 5.0
        if(this->dT > 8)
          this->fStates[1] = Math_easeOut(this->fStates[1], this->dStates[1], 0.69);

        if(Math_dist1d(this->fStates[1], this->dStates[1] * 1.0) < 0.1) 
          this->dStage++;
      break;

      case 1:   // Make the bomb go up

        this->fStates[0] = Math_easeOut(this->fStates[0], this->dStates[0], 0.75);

        if(Math_dist1d(this->fStates[0], this->dStates[0] * 1.0) < 0.001) {
          this->dStates[0] = -(dWidth - dHeight) / 2;
          this->dStates[1] = -(dWidth - dHeight) / 2;

          this->dStage++;
        }
      break;

      case 2:   // Transition into the menu page
        
        // Update the bounding box and the bomb
        this->fStates[0] = Math_easeIn(this->fStates[0], this->dStates[0], 0.975);
        this->fStates[1] = Math_easeIn(this->fStates[1], this->dStates[1], 0.975);

        // Go to next stage
        if(Math_dist1d(this->fStates[0], this->dStates[0] * 1.0) < 0.01 && 
          Math_dist1d(this->fStates[1], this->dStates[1] * 1.0) < 0.01)
          this->dStage++;
      break;

      default:
        this->eAnimationState = ANIMATION_EXIT;
        pPage->ePageState = PAGE_ACTIVE_IDLE;
      break;
    }

    // The minesweeper logo 
    aLogo = AssetManager_getAssetText(pPage->pSharedAssetManager, "logo");

    // Position of the central rectangle
    dRectPos = round(dWidth / 2.0 - dHeight + this->dRoundStates[1] * 2.0);

    /**
     * Writing to the buffer.
     * ......................
    */
    Buffer_contextRect(pBuffer,   // Writable background 
      0, 0, 
      dWidth, dHeight, 
      0xffffff, 0x111111);

    Buffer_contextRect(pBuffer, 
      0, 0,                       // Total black at the edges
      dWidth - 1, dHeight, 
      0x111111, 0x111111);

    Buffer_contextRect(pBuffer,   // A gray bounding box
      dRectPos - 1, 
      this->dRoundStates[1], 
      (dHeight - this->dRoundStates[1] * 2) * 2, 
      dHeight - this->dRoundStates[1] * 2, 
      0x888888, 
      0x888888);

    Buffer_contextRect(pBuffer,   // White canvas in the center
      dRectPos + 1, 
      this->dRoundStates[1] + 1, 
      (dHeight - this->dRoundStates[1] * 2 - 2) * 2, 
      dHeight - this->dRoundStates[1] * 2 - 2,       
      0x111111, 
      0xffffff);

    Buffer_write(pBuffer,         // The mine logo
      round(dWidth / 2.0 - strlen(aLogo[0]) / 2.0), 
      this->dRoundStates[0], 
      dLogoHeight,
      aLogo);
  }
}

/**
 * Handles the menu animations.
 * 
 * @param   { p_obj }   Args_Animation  The animation object that contains our state data.
 * @param   { p_obj }   Args2_Page      The page object we're gonna update based on our state data.
*/
void AnimationHandler_menu(p_obj Args_Animation, p_obj Args2_Page) {

  /**
   * Grab all the pertinent objects and store some constant values.
   * ..............................................................
  */
  Animation *this = (Animation *) Args_Animation;
  Page *pPage = (Page *) Args2_Page;
  Buffer *pBuffer = pPage->pPageBuffer;
 
  int dWidth = pBuffer->dWidth;
  int dHeight = pBuffer->dHeight;
  int i, dCumulativeLength = 0;

  char **aTitleGlyph;                         // We prepend asset variable names with a.
  int dTitleGlyphHeight;                      // The height of a title glyph.

  char sTitleGlyphs[12] = "minesweeper";      // A list of the characters we need from the asset set.
  char dTitleLength = strlen(sTitleGlyphs);   // The length of the title
  char *sGlyphName = String_alloc(16);        // We'll use this to get the specific name of the asset.

  /**
   * Initialization stage.
   * .....................
  */
  if(this->eAnimationState == ANIMATION_INIT) {

    // Init the states we need; these refer to the y positions of each letter
    for(i = 0; i < dTitleLength; i++) {
      this->dStates[i * 2] = 1;
      this->fStates[i * 2] = 0;   // Dummy values so theyre not null to begin with

      this->dStates[i * 2 + 1] = 6;
      this->fStates[i * 2 + 1] = i * i * i * 10.0 - 128.0;
    }

    this->dIntStateCount = (dTitleLength + 1) * 2;
    this->dFloatStateCount = (dTitleLength + 1) * 2;

  /**
   * Update stage.
   * .............
  */
  } else {

    // Update the position of the title along the x axis
    for(i = 0; i < dTitleLength; i++) {
      this->dStates[i * 2] = dCumulativeLength;
      
      // Get the name of the asset
      sprintf(sGlyphName, "main-font-%c", sTitleGlyphs[i]);
      aTitleGlyph = AssetManager_getAssetText(pPage->pSharedAssetManager, sGlyphName);

      // Compute these
      dCumulativeLength += String_length(aTitleGlyph[0]);
    }

    // Shift towards the center
    for(i = 0; i < dTitleLength; i++)
      this->dStates[i * 2] += dWidth / 2 - dCumulativeLength / 2 - 1;

    // Perform different updates based on the stage of the animation
    switch(this->dStage) {

      case 0:   // Make the title go up

        // Make the fStates approach their targets (this is along the y-axis)
        for(i = 0; i < dTitleLength; i++)
          this->fStates[i * 2 + 1] = Math_easeOut(this->fStates[i * 2 + 1], this->dStates[i * 2 + 1], 0.75);

        // Go to next stage
        if(Math_dist1d(this->fStates[1], this->dStates[1] * 1.0) < 0.001) {          
          this->dStage++;
        }
      break;

      case 1:   // We chillin

      break;

      default:
        this->eAnimationState = ANIMATION_EXIT;
        pPage->ePageState = PAGE_ACTIVE_IDLE;
      break;
    }
    
    /**
     * Writing to the buffer.
     * ......................
    */
    Buffer_contextRect(pBuffer,   // Background
      0, 0, 
      dWidth, dHeight, 
      0x212121, 0xffffff);

    // Title
    for(i = 0; i < dTitleLength; i++) {
      
      // Get the glyph asset
      sprintf(sGlyphName, "main-font-%c", sTitleGlyphs[i]);
      aTitleGlyph = AssetManager_getAssetText(pPage->pSharedAssetManager, sGlyphName);
      dTitleGlyphHeight = AssetManager_getAssetHeight(pPage->pSharedAssetManager, sGlyphName);
      
      // Display the letter
      Buffer_write(pBuffer,
        this->dStates[i * 2],
        this->dRoundStates[i * 2 + 1],
        
        dTitleGlyphHeight, 
        aTitleGlyph);
    }
  }
}

#endif
