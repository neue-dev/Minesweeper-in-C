/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-02-28 22:17:06
 * @ Description:
 * 
 * This file contains definitions for animation handlers (basically,
 *    functions that increment the animation over time).
 */

#include "./utils/utils.animation.h"
#include "./utils/utils.buffer.h"
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

void AnimationHandler_intro(p_obj Args_Animation, p_obj Args2_Buffer) {
  int i;
  int dWidth = IO_getWidth();
  int dHeight = IO_getHeight();
  
  Animation *this = (Animation *) Args_Animation;
  Buffer *pBuffer = (Buffer *) Args2_Buffer;

  for(i = 0; i < 6; i++)
    this->fStates[i] = Math_ease(this->fStates[i], this->dStates[i], 0.1);

  // Once text is centered
  if(this->dRoundStates[4] == this->dStates[4] && this->dStates[6] < 32)
    this->dStates[6]++;

  if(this->dT % 24 == 0) {
    this->dStates[0] = rand() % IO_getWidth();
    this->dStates[1] = rand() % IO_getHeight();
    
  } else if(this->dT % 24 == 12) {
    this->dStates[2] = rand() % IO_getWidth();
    this->dStates[3] = rand() % IO_getHeight();
  }

  // Display update
  char *block[8] = {
      "              _                                                   ",
      "   ____ ___  (_)___  ___  ______      _____  ___  ____  ___  _____",
      "  / __ `__ \\/ / __ \\/ _ \\/ ___/ | /| / / _ \\/ _ \\/ __ \\/ _ \\/ ___/",
      " / / / / / / / / / /  __(__  )| |/ |/ /  __/  __/ /_/ /  __/ /    ",
      "/_/ /_/ /_/_/_/ /_/\\___/____/ |__/|__/\\___/\\___/ .___/\\___/_/     ",
      "                                              /_/                 ",
      "                                                                  ",
      "                                                                  ",
    };

  Buffer_context(pBuffer, 0, 10 - this->dStates[6] / 2, IO_getWidth(), this->dStates[6] + 10, 0x000000, 0xffffff);

  Buffer_context(pBuffer, 
    10, 5,
    dWidth - 20, dHeight - 10, 
    0xffffff, 0xdd0000);

  Buffer_context(pBuffer, 
    12, 6,
    dWidth - 24, dHeight - 12, 
    0x000000, 0xffffff);

  Buffer_context(pBuffer, 
    this->dRoundStates[0], 
    this->dRoundStates[1],
    10, 5, 0xffffff, 0x0000dd);

  Buffer_context(pBuffer, 
    this->dRoundStates[2], 
    this->dRoundStates[3],
    10, 5, 0xffffff, 0xdd0000);

  Buffer_write(pBuffer, 
    this->dRoundStates[4], 
    this->dRoundStates[5], 
    strlen(block[0]), 8, block);
}
