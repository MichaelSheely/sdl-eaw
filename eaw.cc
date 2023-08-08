#include <stdint.h>
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "draw_circle.h"

// Can also use 1280x720.
#define WINDOW_WIDTH   1920
#define WINDOW_HEIGHT  1080
static const char kWindowTitle[] = "SDL Test";
// SDL_WINDOW_FULLSCREEN SDL_WINDOW_FULLSCREEN_DESKTOP SDL_WINDOW_MAXIMIZED
static uint32_t g_win_flags = SDL_WINDOW_RESIZABLE;
static SDL_Window* g_window;
static SDL_Renderer* renderer;
static SDL_Texture* acc_texture;
const char kGCMapFilename[] = "gc_map_placeholder.jpg";
const char kAcclamatorFilename[] = "acclamator.png";

struct LaunchFlags {
  std::string assets_directory;
  std::string typeface_path;
};

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

int DrawAcclamatorTriangle(const std::string& assets_directory) {
  SDL_Rect rect;
  rect.w = 789 / 2;
  rect.h = 400 / 2;
  rect.x = 640/2;
  rect.y = 480/2;

  return SDL_RenderCopy(renderer, acc_texture, NULL, &rect);
}

int DrawBasicTriangle() {
  // Alternatively, could use SDL_PIXELFORMAT_RGB555.
  // SDL_Texture* texture = SDL_CreateTexture(
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

// For true 3d rendering, see:
// https://www.khronos.org/opengl/wiki/Tutorial3:_Rendering_3D_Objects_(C_/SDL)

int RenderRectangleOrTriangle(
    const std::string& assets_directory, bool draw_rectangle) {
  int result;
  if (draw_rectangle) {
    SDL_Rect rect;
    DefaultRect(&rect);
    result = SDL_RenderFillRect(renderer, &rect);
  } else {
    result = DrawAcclamatorTriangle(assets_directory);
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
  return SDL_RenderCopy(renderer, texture, NULL, NULL);
}

// Cribbing from https://stackoverflow.com/a/38169008/6472082.
void display_text(int x, int y, char* text, TTF_Font* font) {
  int text_width;
  int text_height;
  SDL_Surface* surface;
  SDL_Color textColor = {255, 255, 255, 0};

  surface = TTF_RenderText_Solid(font, text, textColor);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  text_width = surface->w;
  text_height = surface->h;
  SDL_FreeSurface(surface);
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = text_width;
  rect.h = text_height;

  if (SDL_RenderCopy(renderer, texture, NULL, &rect) != 0) {
    printf("Failed to write text due to: %s\n", SDL_GetError());
  }
}

int launch_game(const LaunchFlags& launch_flags) {
  // set up SDL
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
    printf("Failed to init SDL: %s\n", SDL_GetError());
    return 1;
  }
  // Can also OR together IMG_INIT_PNG and IMG_INIT_TIF if needed.
  IMG_Init(IMG_INIT_JPG);
  TTF_Init();
  TTF_Font* font = TTF_OpenFont(launch_flags.typeface_path.c_str(), 12);
  if (font == NULL) {
    fprintf(stderr, "Failed to open droid sans font\n", SDL_GetError());
    return 1;
  }

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

  acc_texture = IMG_LoadTexture(
      renderer, (launch_flags.assets_directory + kAcclamatorFilename).c_str());

  SDL_Event event;
  bool running = true;
  bool draw_rectangle = true;

  SDL_ShowCursor(SDL_ENABLE /* or SDL_DISABLE if mouse not needed*/);
  while (running) {
    // Clear any garbage which was pre-existing in the rendering flow.
    SDL_SetRenderDrawColor(renderer, 0, 0, 150, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 0);

    // Every frame should poll until all events are handled.
    while (SDL_PollEvent(&event)) {
      // Update whether or not we are still running and what to draw
      // based on user input.
      HandleEvent(&event, &running, &draw_rectangle);
    }

    if (RenderGCBackground(launch_flags.assets_directory) != 0) {
      return 1;
    }

    if (RenderRectangleOrTriangle(
        launch_flags.assets_directory, draw_rectangle) != 0) {
      printf("Failed to render: %s\n", SDL_GetError());
      return 1;
    }

    int x,y;
    SDL_GetMouseState(&x, &y);
    char mouse_position[30];
    SDL_SetRenderDrawColor(renderer, 100, 100, 0, 0);
    sprintf(mouse_position, "(%d,%d)", x, y);
    display_text(0, 0, mouse_position, font);
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

// Attempts to extract the given flag from the element
// of argv[] to flag_value.  If successful, returns true.
bool attempt_flag_extraction(
    const std::string& element_of_argv,
    const std::string& desired_flag,
    std::string* flag_value) {
  size_t desired_flag_length = desired_flag.length();
  if (element_of_argv.rfind(desired_flag, 0) == 0) {
    *flag_value = element_of_argv.substr(
        desired_flag_length, element_of_argv.length() - desired_flag_length);
    return true;
  }
  return false;
}

int main(int argc, char** argv) {
  LaunchFlags launch_flags;
  for (int i = 0; i < argc; ++i) {
    std::string option = argv[i];
    attempt_flag_extraction(
        option, "--assets=", &launch_flags.assets_directory);
    attempt_flag_extraction(
        option, "--typeface=", &launch_flags.typeface_path);
  }
  if (launch_flags.assets_directory.empty()) {
    printf("Must provide the directory from which to load assets via `--assets=`.\n");
    return 1;
  }
  if (launch_flags.typeface_path.empty()) {
    printf("Must provide the path to the typeface to use via `--typeface=`.\n");
    return 1;
  }
  launch_game(launch_flags);
  return 0;
}
