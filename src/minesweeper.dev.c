/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-02-27 08:50:40
 * @ Description:
 * 
 * The main game file.
 */

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.buffer.h"
#include "utils/utils.grid.h"
#include "utils/utils.io.h"

#include <stdio.h>

int main() {

  Buffer *pBuffer = Buffer_create(
    IO_getWidth(), 
    IO_getHeight(), 
    Graphics_getCodeFG(0x000000), 
    Graphics_getCodeBG(0xffffff));

  char *block[8] = {
    "hello world!aaaaaaaaaaaa",
    "this is an array of strings",
    "idk man10101010aaaaa",
    "yeppers10101010aaaa",
    "idk man10101010aaa",
    "idk man10101010aaaaa",
    "yeppers10101010aaaa",
    "idk man10101010aaa",
  };
  Buffer_write(pBuffer, 10, 10, strlen(Graphics_getCodeFG(0xff0000)), 8, block);
  Buffer_context(pBuffer, 10, 10, 10, 10, Graphics_getCodeFGBG(0xff0000, 0x0000dd));
  Buffer_context(pBuffer, 12, 12, 10, 5, Graphics_getCodeFGBG(0x00ff00, 0xff00dd));

  IO_clear();
  Buffer_print(pBuffer);
  free(pBuffer);

  // for(int i = 0; i < pBuffer->dHeight; i++) {
  //   for(int j = 0; j < pBuffer->dWidth; j++) {
  //     printf("%d",pBuffer->dContextMask[i][j]);
  //   }
  //   printf("\n");
  // }

  while(1);

  return 0;
}