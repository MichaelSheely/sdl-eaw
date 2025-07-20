#ifndef EAW_SPACE_OBJECT_H_
#define EAW_SPACE_OBJECT_H_
#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define PI 3.14159265
#define FLOAT_COMPARATOR 0.2

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

float Distance(SDL_Point p1, SDL_Point p2) {
  float dx = static_cast<float>(p1.x - p2.x);
  float dy = static_cast<float>(p1.y - p2.y);
  return sqrt(dx * dx + dy * dy);
}

float AngleBetween(float radian1, float radian2) {
  float diff = fmod(radian2 - radian1, 2 * PI);
  if (diff > PI) {
    diff -= 2 * PI;
  }
  if (diff < -PI) {
    diff += 2 * PI;
  }
  return diff;
}

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
  float draw_rotation;
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
  // Returns log messages.
  std::vector<std::string> UpdateLocationAndVelocity(
      bool verbose_movement_logging) {
    std::vector<std::string> log_messages;
    float distance_to_waypoint;
    if (waypoints.empty()) {
      if (speed > FLOAT_COMPARATOR) {
        speed = speed - acceleration;
      } else {
        speed = 0.0;
      }
      char buffer[100];
      int size = sprintf(buffer, "No destination, speed is %.2f", speed);
      std::string speed_log = std::string(buffer);
      speed_log.resize(size);
      log_messages.push_back(speed_log);
      return log_messages;
    } else {
      distance_to_waypoint = Distance(waypoints[waypoints.size() - 1], center_position);
      if (distance_to_waypoint < 30.0) {
        // Close enough to the destination, consider it achieved.
        waypoints.pop_back();
        printf("Made it to destination, removing waypoint. Remaining waypoints %d\n",
               waypoints.size());
        return log_messages;
      }
      char buffer[100];
      int size = sprintf(
          buffer, "Distance to waypoint is %.2f, speed is %.2f",
          Distance(waypoints[waypoints.size() - 1], center_position), speed);
      std::string distance_remaining = std::string(buffer);
      distance_remaining.resize(size);
      log_messages.push_back(distance_remaining);
    }

    SDL_Point goal = waypoints[waypoints.size() - 1];
    int delta_x = goal.x - center_position.x;
    int delta_y = goal.y - center_position.y;

    // TODO: Double check calculation here, depending on the
    // domain / range of builtin atan, may need to adjust
    // for quadrants.
    float desired_radians = static_cast<float>(
        // Negate y value because we draw with an inverted y axis.
        atan2(static_cast<double>(-delta_y), static_cast<double>(delta_x)));

    char buffer[100];
    int size = sprintf(
        buffer, "Desired heading %.2f radians (%.2f degrees)",
        desired_radians, (desired_radians * 180 ) / PI);
    std::string heading_log = std::string(buffer);
    heading_log.resize(size);
    log_messages.push_back(heading_log);
    size = sprintf(
        buffer, "Current heading %.2f radians (%.2f degrees)",
        heading, (heading * 180 ) / PI);
    // Trim extra chars.
    heading_log = std::string(buffer);
    heading_log.resize(size);
    log_messages.push_back(heading_log);

    // Heading is close enough to start accelerating.
    float angle = AngleBetween(heading, desired_radians);
    if (verbose_movement_logging) {
      printf("delta_x: %d, delta_y: %d, heading: %.2f, "
             "desired_radians %.2f, angle %.2f\n",
             delta_x, delta_y, heading, desired_radians, angle);
    }

    size = sprintf(
        buffer, "Need to move %.2f radians in %s direction",
        fabs(angle), angle < 0 ? "clockwise" : "counter clockwise");
    // Trim extra chars.
    heading_log = std::string(buffer);
    heading_log.resize(size);
    log_messages.push_back(heading_log);
    // TODO: Split into two braches.  If we are very close, set the value.
    // If we are somewhat close, allow movement while turning continues.
    if (fabs(angle) < 0.01 || fabs(angle) < rotational_acceleration) {
      // Close enough to the right heading, no need for further rotation.
      // Also accelerate toward our destination.
      speed += acceleration;
      heading = desired_radians;
    } else {
      // Otherwise, turn.
      heading += rotational_acceleration * (angle < 0 ? -1 : 1);
      if (heading >= PI) {
        heading -= 2 * PI;
      }
      if (heading <= -PI) {
        heading += 2 * PI;
      }
      char buffer[100];
      int size = sprintf(
          buffer, "Heading was not close enough (%.2f vs %.2f) diff: %.2f, need to keep turning.",
          desired_radians, heading, angle);
      std::string heading_diff_log = std::string(buffer);
      heading_diff_log.resize(size);
      log_messages.push_back(heading_diff_log);
    }

    speed = std::min(speed, max_speed);

    // If we are getting too close to our destination, cut the acceleration.
    // k = time at which to cut the acceleration
    // k = D - (v_0 * v_0 / (2a)) / v_0.

    center_position.x += speed * cos(heading);
    // Reverse the y of motion for drawing.
    center_position.y -= speed * sin(heading);

    //  char buffer[100];
    //  int size = sprintf(
    //      buffer, "Heading diff was %.2f which was less than %.2f)",
    //      angle, FLOAT_COMPARATOR);
    //  std::string heading_diff_log = std::string(buffer);
    //  heading_diff_log.resize(size);
    //  log_messages.push_back(heading_diff_log);
    return log_messages;
  }
};

#endif  // EAW_SPACE_OBJECT_H_
