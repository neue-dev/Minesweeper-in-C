/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-25 19:43:14
 * @ Modified time: 2024-03-25 20:16:31
 * @ Description:
 * 
 * Stores some important settings for keybinds and what not.
 */

#ifndef SETTINGS_
#define SETTINGS_

#include "./utils/utils.event.h"
#include "./utils/utils.theme.h"

/**
 * Sets certain keybinds in the event store.
 * Sets the current theme in the theme manager.
 * 
 * @param   { EventStore * }    pSharedEventStore     The event store we're using for the engine.
 * @param   { ThemeManager * }  pSharedThemeManager   The theme manager we have for the engine.
*/
void Settings_init(EventStore *pSharedEventStore, ThemeManager *pSharedThemeManager) {
  
  // Movement keybinds
  EventStore_set(pSharedEventStore, "game-move-up", 'w');
  EventStore_set(pSharedEventStore, "game-move-down", 's');
  EventStore_set(pSharedEventStore, "game-move-left", 'a');
  EventStore_set(pSharedEventStore, "game-move-right", 'd');

  // Other game keybinds
  EventStore_set(pSharedEventStore, "game-toggle-flag", 'f');
  EventStore_set(pSharedEventStore, "game-edit-cell", '\n');

  // Misc keybinds
  EventStore_set(pSharedEventStore, "game-toggle-pause", 'p');
  EventStore_set(pSharedEventStore, "game-quit", 'q');

  // Default theming
  ThemeManager_setActive(pSharedThemeManager, "default");
}

/**
 * Tells us what keybinds we have for the game.
 * This function assumes that there is enough space on the output buffer.
 * 
 * @param   { int * }     dKeybindCount     The number of keybinds we're using for the game.
 * @param   { char * }    sKeybindArray[]   An output buffer where we store the list of the keybind names.
*/
void Settings_getKeybinds(int *dKeybindCount, char *sKeybindArray[]) {

  sKeybindArray[0] = "game-move-up";
  sKeybindArray[1] = "game-move-down";
  sKeybindArray[2] = "game-move-left";
  sKeybindArray[3] = "game-move-right";
  sKeybindArray[4] = "game-toggle-flag";
  sKeybindArray[5] = "game-edit-cell";
  sKeybindArray[6] = "game-toggle-pause";
  sKeybindArray[7] = "game-quit";

  *dKeybindCount = 8;
}

#endif

