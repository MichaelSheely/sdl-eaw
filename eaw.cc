#include <stdint.h>
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include "draw_circle.h"

// Can also use 1280x720.
#define WINDOW_WIDTH   1920
#define WINDOW_HEIGHT  1080
static const char kWindowTitle[] = "SDL Test";
// SDL_WINDOW_FULLSCREEN SDL_WINDOW_FULLSCREEN_DESKTOP SDL_WINDOW_MAXIMIZED
static uint32_t g_win_flags = SDL_WINDOW_RESIZABLE;
static SDL_Window* g_window;
static SDL_Renderer* renderer;
const char kGCMapFilename[] = "gc_map_placeholder.jpg";

// Returns if still running.
void HandleInput(int keyCode, int modCode, bool pressed) {
  return;
}

void HandleEvent(SDL_Event* event, bool* running, bool* draw_rectangle) {
  switch(event->type) {
    case SDL_CONTROLLERDEVICEADDED:
      // OpenOneGamepad(event->cdevice.which);
      break;
    case SDL_CONTROLLERAXISMOTION:
      // HandleGamepadAxisInput(event->caxis.which, event->caxis.axis, event->caxis.value);
      break;
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP: {
      // int b = RemapSdlButton(event->cbutton.button);
      // if (b >= 0) {
      //   HandleGamepadInput(b, event->type == SDL_CONTROLLERBUTTONDOWN);
      // }
      break;
    }
    case SDL_MOUSEWHEEL:
      if (SDL_GetModState() & KMOD_CTRL && event->wheel.y != 0) {
        // ChangeWindowScale(event->wheel.y > 0 ? 1 : -1);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      // if (event->button.button == SDL_BUTTON_LEFT &&
      //     event->button.state == SDL_PRESSED &&
      //     event->button.clicks == 2) {
      //   if ((g_win_flags & SDL_WINDOW_FULLSCREEN_DESKTOP) == 0 &&
      //       (g_win_flags & SDL_WINDOW_FULLSCREEN) == 0 &&
      //       SDL_GetModState() & KMOD_SHIFT) {
      //     g_win_flags ^= SDL_WINDOW_BORDERLESS;
      //     SDL_SetWindowBordered(
      //         g_window, (g_win_flags & SDL_WINDOW_BORDERLESS) == 0);
      //   }
      // }
      break;
    case SDL_KEYDOWN:
      if (event->key.keysym.sym == SDLK_r ) {
        *draw_rectangle = true;
      } else if (event->key.keysym.sym == SDLK_t) {
        *draw_rectangle = false;
      } else if (event->key.keysym.sym == SDLK_q) {
        *running = false;
      }
      /*
      printf("Other keycode; Physical %s key acting as %s key\n",
             SDL_GetScancodeName(event->key.keysym.scancode),
             SDL_GetKeyName(event->key.keysym.sym));
      */
      HandleInput(event->key.keysym.sym, event->key.keysym.mod, true);
      break;
    case SDL_KEYUP:
      HandleInput(event->key.keysym.sym, event->key.keysym.mod, false);
      break;
    case SDL_QUIT:
      *running = false;
      break;
    }
}

void DefaultRect(SDL_Rect* rect) {
  rect->w = 200;
  rect->h = 200;
  rect->x = 640/2;
  rect->y = 480/2;
}

int TryDrawTriangle() {
  // SDL_Texture* texture = SDL_CreateTexture(
  //     // renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_STATIC, 3, 3);
  //     renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_STATIC, 3, 3);

  SDL_Vertex vertices[3];
  vertices[0].position.x = 100;
  vertices[0].position.y = 120;
  vertices[1].position.x = 200;
  vertices[1].position.y = 250;
  vertices[2].position.x = 200;
  vertices[2].position.y = 90;

  // For https://wiki.libsdl.org/SDL2/SDL_SetRenderDrawBlendMode,
  // see https://en.wikipedia.org/wiki/Alpha_compositing
  // and https://en.wikipedia.org/wiki/Blend_modes

  // SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  if (SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0) != 0) {
    printf("Failed to render triangle: %s\n", SDL_GetError());
    return 1;
  }
  return 0;
}

int RenderRectangleOrTriangle(bool draw_rectangle) {
  int result;
  if (draw_rectangle) {
    SDL_Rect rect;
    DefaultRect(&rect);
    result = SDL_RenderFillRect(renderer, &rect);
  } else {
    result = TryDrawTriangle();
  }
  return result;
}

int RenderBlueCircle(
    SDL_Renderer* renderer, int x, int y, int r) {
  return ellipseRGBA(renderer, x, y, r, r, 0, 100, 0, 0);
}

int RenderGCBackground(const std::string& assets_directory) {
  SDL_Texture* texture = NULL;
  texture = IMG_LoadTexture(
      renderer, (assets_directory + kGCMapFilename).c_str());
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  return 0;
}

int launch_game(const std::string& assets_directory) {
  // set up SDL
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
    printf("Failed to init SDL: %s\n", SDL_GetError());
    return 1;
  }
  // Can also OR together IMG_INIT_PNG and IMG_INIT_TIF if needed.
  IMG_Init(IMG_INIT_JPG);

  g_window = SDL_CreateWindow(
      kWindowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (g_window == NULL) {
    fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
    return 1;
  }
  // Create a gpu renderer associated with the given window.
  renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    fprintf(stderr, "Unable to create GPU renderer for the given window: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Event event;
  bool running = true;
  bool draw_rectangle = true;

  SDL_ShowCursor(SDL_ENABLE /* or SDL_DISABLE if mouse not needed*/);
  while (running) {
    // Clear any garbage which was pre-existing in the rendering flow.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 0);

    // Every frame should poll until all events are handled.
    while (SDL_PollEvent(&event)) {
      // Update whether or not we are still running and what to draw
      // based on user input.
      HandleEvent(&event, &running, &draw_rectangle);
    }

    if (RenderGCBackground(assets_directory) != 0) {
      return 1;
    }

    if (RenderRectangleOrTriangle(draw_rectangle) != 0) {
      return 1;
    }

    int x,y;
    SDL_GetMouseState(&x, &y);
    SDL_Rect rect;
    rect.w = 10;
    rect.h = 10;
    rect.x = x - 10;
    rect.y = y - 10;

    if (SDL_RenderFillRect(renderer, &rect) != 0) {
      return 1;
    }
    if (RenderBlueCircle(renderer, x, y, 30) != 0) {
      return 1;
    }

    // After all drawing has been completed, present rendering via GPU.
    SDL_RenderPresent(renderer);

    int milliseconds = 10;
    SDL_Delay(milliseconds);
  }
  /* Frees memory */
  SDL_DestroyWindow(g_window);
  /* Shuts down all SDL subsystems */
  SDL_Quit();
  IMG_Quit();
  return 0;
}

int main(int argc, char** argv) {
  std::string assets_directory;
  for (int i = 0; i < argc; ++i) {
    std::string option = argv[i];
    if (option.rfind("--assets=", 0) == 0) {
      assets_directory = option.substr(9, option.length() - 9);
    }
  }
  if (assets_directory.empty()) {
    printf("Must provide the directory from which to load assets via `--assets=`.\n");
    return 1;
  }
  launch_game(assets_directory);
  return 0;
}
