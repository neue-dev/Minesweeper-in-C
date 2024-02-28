/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-02-28 19:36:28
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
  Animation *this = (Animation *) Args_Animation;
  Buffer *pBuffer = (Buffer *) Args2_Buffer;

  this->fStates[0] -= (this->fStates[0] - this->dStates[0]) / 5;
  this->fStates[1] -= (this->fStates[1] - this->dStates[1]) / 5;

  this->fStates[2] -= (this->fStates[2] - this->dStates[2]) / 5;
  this->fStates[3] -= (this->fStates[3] - this->dStates[3]) / 5;

  // The logo
  this->fStates[4] -= (this->fStates[4] - this->dStates[4]) / 5;
  this->fStates[5] -= (this->fStates[5] - this->dStates[5]) / 5;

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
  char *block[5] = {
      "    __  ________   ______________       __________________  __________ ",
      "   /  |/  /  _/ | / / ____/ ___/ |     / / ____/ ____/ __ \\/ ____/ __ \\",
      "  / /|_/ // //  |/ / __/  \\__ \\| | /| / / __/ / __/ / /_/ / __/ / /_/ /",
      " / /  / // // /|  / /___ ___/ /| |/ |/ / /___/ /___/ ____/ /___/ _, _/ ",
      "/_/  /_/___/_/ |_/_____//____/ |__/|__/_____/_____/_/   /_____/_/ |_|  ",
    };

  Buffer_context(pBuffer, 
    this->dRoundStates[0], 
    this->dRoundStates[1],
    10, 5, -1, 0x0000dd);

  Buffer_context(pBuffer, 
    this->dRoundStates[2], 
    this->dRoundStates[3],
    10, 5, -1, 0xdd0000);
  Buffer_write(pBuffer, 
    this->dRoundStates[4], 
    this->dRoundStates[5], 
    strlen(block[0]), 5, block);
  Buffer_context(pBuffer, 0, 11 - this->dStates[6] / 2, IO_getWidth(), this->dStates[6] + 8, 0x000000, 0xffffff);
}
