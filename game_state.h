#ifndef SDL_EAW_GAME_STATE_H_
#define SDL_EAW_GAME_STATE_H_
#include <chrono>
#include <string>
#include "space_object.h"

struct GameState {
  enum class CommandMode {
    kSpaceTacticalView,
    kGalacticOverivew,
    kPlanetView,
    // TODO: Add planetary base details.
  };
  const LaunchFlags* launch_flags;
  CommandMode mode;
  bool running = true;
  bool paused = false;
  bool spawn_acc = false;

  std::chrono::microseconds last_frame;
  std::chrono::microseconds last_frame_render;

  std::vector<std::string> messages_to_display;

  struct ItemUnderCursor {
    enum class ItemType {
      kSpaceObject,
    };
    // The type of item under the cursor.
    ItemType item_type;
    // We will maintain a map of items of each type which allows
    // lookup by item id.
    std::string item_key;
    // Count of the game ticks during which the item was under the cursor.
    int game_ticks_under_cursor = 0;
  };
  ItemUnderCursor item_under_cursor;


  struct SpaceTacticalState {
    // TODO: Consider converting to a map from layer to objects.
    // This will ensure data locality when it comes to drawing.
    // TODO: Convert to flat map from object id to object so we can
    // get constant time lookup.
    std::vector<SpaceObject> objects;
  };
  SpaceTacticalState tactical_state;
};

#endif  // SDL_EAW_GAME_STATE_H_
