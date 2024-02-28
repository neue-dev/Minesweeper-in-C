/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-02-28 10:44:21
 * @ Description:
 * 
 * The main game file.
 */

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.animation.h"
#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.buffer.h"
#include "utils/utils.grid.h"
#include "utils/utils.io.h"

#include <stdio.h>

int main() {

  Animation *pAnim = Animation_create(
    "intro-animation",
    
    AnimationHandler_intro, 
    
    4,            // 4 states to initialize
    'i', 420,     // first int state, 420 initial value
    'i', 69,      // second int state, 69 initial value
    'f', 0.123,   // first float state, 0.123 initial value
    'f', 0.456);  // second float state, 0.456 initial value

  for(int i = 0; i < pAnim->dFloatStateCount; i++)
    printf("%f hmm", pAnim->fStates[i]);
  printf("\n");

  for(int i = 0; i < pAnim->dIntStateCount; i++)
    printf("%d aha", pAnim->dStates[i]);
  printf("\n");

  while(pAnim->dStates[0] < 430) {
    Animation_update(pAnim);

    for(int i = 0; i < pAnim->dIntStateCount; i++)
      printf("%d ", pAnim->dStates[i]);
    printf("\n");

  };

  while(1);

  return 0;
}