#ifndef EAW_SPACE_OBJECT_H_
#define EAW_SPACE_OBJECT_H_
#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "game_state.h"

#define PI 3.14159265

// Specifies the layer in which the draw elements.
enum Layer {
  // No units allowed on this layer.
  kBackgroundLayer = 0,
  // Space stations.
  kSuperstructureLayer,
  // Exceptionally large naval vessels, such as
  // super star destroyers.
  kDreadnoughtLayer,
  // For standard capital ships such as ISDs and
  // Mon Calamari cruisers.
  kCapitalShipLayer,
  // Used by both Frigates and Cruisers.
  kFrigateCruiserLayer,
  // Used by Corvettes.
  // May consider merging with Frigates and Cruisers.
  kCorvetteLayer,
  // Top layer, consisting of small fighter craft.
  kFighterLayer,
};

// TODO Allow assigning fleet commanders to ships.
// When selecting a unit which can act as a commander,
// there will be an option to move them to any slot
// under a non-fighter ship.
// Special case vader, he must be available before
// the Executor can be built.

struct SpaceObject {
  SDL_Point center_position;
  std::string object_id;
  std::string object_class;
  std::string object_name;
  SDL_Texture* texture;
  int width;
  int height;
  Layer layer;
  // In radians, from -pi to pi.
  float heading;
  float acceleration;
  float rotational_acceleration;
  float max_speed;
  float speed = 0.0;
  // If empty, we're already where we want to be.
  std::vector<SDL_Point> waypoints;
  bool selected = false;
  void GetBoundingBox(SDL_Rect* rect) const {
    rect->x = center_position.x - (width / 2);
    rect->y = center_position.y - (height / 2);
    rect->w = width;
    rect->h = height;
  }
  void Select() {
    selected = true;
  }
  void Deselect() {
    selected = false;
  }
  std::string UpdateLocationAndVelocity() {
    if (waypoints.empty()) {
      if (speed > 0.01) {
        speed = speed - acceleration;
      } else {
        speed = 0.0;
      }
      return "test";
    }

    SDL_Point goal = waypoints[waypoints.size() - 1];
    int delta_x = goal.x - center_position.x;
    int delta_y = goal.y - center_position.y;

    // TODO: Double check calculation here, depending on the
    // domain / range of builtin atan, may need to adjust
    // for quadrants.
    float desired_radians = static_cast<float>(
        atan(static_cast<double>(delta_x) / delta_y));
    char buffer[100];
    int size = sprintf(
        buffer, "Desired heading %.2f radians (%.2f degrees)",
        desired_radians, (desired_radians * 180 ) / PI);
    std::string heading_log = std::string(buffer);
    // Trim extra chars.
    heading_log.resize(size);
    return heading_log;
  }
};

#endif  // EAW_SPACE_OBJECT_H_
