/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-03-02 21:34:59
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
  int i, cumulativeLen = 0;
  int dWidth = IO_getWidth();
  int dHeight = IO_getHeight();
  
  Animation *this = (Animation *) Args_Animation;
  Buffer *pBuffer = (Buffer *) Args2_Buffer;

  char *title[11][6] = {
    {
      "███╗   ███╗",
      "████╗ ████║",
      "██╔████╔██║",
      "██║╚██╔╝██║",
      "██║ ╚═╝ ██║",
      "╚═╝     ╚═╝",
    },
    {
      "██╗",
      "██║",
      "██║",
      "██║",
      "██║",
      "╚═╝",
    },
    {
      "███╗   ██╗",
      "████╗  ██║",
      "██╔██╗ ██║",
      "██║╚██╗██║",
      "██║ ╚████║",
      "╚═╝  ╚═══╝",
    },
    {
      "███████╗",
      "██╔════╝",
      "█████╗  ",
      "██╔══╝  ",
      "███████╗",
      "╚══════╝",
    },
    {
      "███████╗",
      "██╔════╝",
      "███████╗",
      "╚════██║",
      "███████║",
      "╚══════╝",
    },
    {
      "██╗    ██╗",
      "██║    ██║",
      "██║ █╗ ██║",
      "██║███╗██║",
      "╚███╔███╔╝",
      " ╚══╝╚══╝ ",
    },
    {
      "███████╗",
      "██╔════╝",
      "█████╗  ",
      "██╔══╝  ",
      "███████╗",
      "╚══════╝",
    },
    {
      "███████╗",
      "██╔════╝",
      "█████╗  ",
      "██╔══╝  ",
      "███████╗",
      "╚══════╝",
    },
    {
      "██████╗ ",
      "██╔══██╗",
      "██████╔╝",
      "██╔═══╝ ",
      "██║     ",
      "╚═╝     ",
    },
    {
      "███████╗",
      "██╔════╝",
      "█████╗  ",
      "██╔══╝  ",
      "███████╗",
      "╚══════╝",
    },
    {
      "██████╗ ",
      "██╔══██╗",
      "██████╔╝",
      "██╔══██╗",
      "██║  ██║",
      "╚═╝  ╚═╝",
    }
  };

  // Initialization stage
  if(!this->bIsInitted) {

    // Initialize the states
    for(i = 0; i < 4; i++) {
      this->fStates[i] = 0.0;
      this->dStates[i] = 0;
    }

    for(i = 0; i < 11; i++) {
      this->dStates[i * 2 + 4] = cumulativeLen + 24;
      this->fStates[i * 2 + 4] = cumulativeLen * 5.0 - 50.0;
      
      this->dStates[i * 2 + 1 + 4] = 12;
      this->fStates[i * 2 + 1 + 4] = i * i * i * 10.0 - 128.0;

      cumulativeLen += String_length(title[i][0]);
    }


    // You have to manually set these cuz u didnt pass a statecount in the constuctor
    this->dFloatStateCount = 26;
    this->dIntStateCount = 26;

  // Running the animation
  } else {
    for(i = 0; i < 26; i++)
      this->fStates[i] = Math_ease(this->fStates[i], this->dStates[i], 0.69);

    // Randomize square positions
    if(this->dT % 24 == 0) {
      this->dStates[0] = rand() % IO_getWidth();
      this->dStates[1] = rand() % IO_getHeight();
      
    } else if(this->dT % 24 == 12) {
      this->dStates[2] = rand() % IO_getWidth();
      this->dStates[3] = rand() % IO_getHeight();
      
    } else if(this->dT % 60 == 59) {
      
      for(i = 0; i < 11; i++) {
        this->fStates[i * 2 + 4] = rand() % IO_getWidth() + IO_getWidth() * ((rand() % 2) * 2 - 1);
        this->fStates[i * 2 + 1 + 4] = rand() % IO_getHeight() + IO_getHeight() * ((rand() % 2) * 2 - 1);
      }
    }

    Buffer_contextRect(pBuffer, 
      0, 0,
      dWidth, dHeight, 
      0xffffff, 0x000000);

    Buffer_contextRect(pBuffer, 
      0, 0,
      dWidth - 1, dHeight, 
      0x000000, 0x000000);

    Buffer_contextRect(pBuffer, 
      10, 5,
      dWidth - 20, dHeight - 10, 
      0xdd2121, 0xdd2121);

    Buffer_contextRect(pBuffer, 
      12, 6,
      dWidth - 24, dHeight - 12, 
      0x000000, 0xffffff);

    // Fun squares
    Buffer_contextRect(pBuffer, 
      this->dRoundStates[0], 
      this->dRoundStates[1],
      10, 5, 0xffffff, 0x0000dd);

    Buffer_contextCircle(pBuffer, 
      this->dRoundStates[2], 
      this->dRoundStates[3],
      3, 0x000000, 0xcccccc);

    for(i = 0; i < 11; i++) {
      Buffer_write(pBuffer, 
        this->dStates[4 + i * 2], this->dRoundStates[4 + i * 2 + 1], 
        6, title[i]);
    }
  }
}
