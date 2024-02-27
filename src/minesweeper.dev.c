/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-02-27 10:04:34
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

  Animation *pAnim = Animation_create(4, 
    'i', 420,
    'i', 69,
    'f', 0.123,
    'f', 0.456);

  for(int i = 0; i < pAnim->dFloatStateCount; i++)
    printf("%f hmm", pAnim->fStates[i]);
  printf("\n");

  for(int i = 0; i < pAnim->dIntStateCount; i++)
    printf("%d aha", pAnim->dStates[i]);
  printf("\n");

  while(1);

  return 0;
}