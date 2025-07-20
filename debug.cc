#include <SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "INIT VIDEO FAILURE.\n";
    return 1;
  }
  SDL_Window* window = SDL_CreateWindow(
      "TestWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      640 * 2, 480 * 2, SDL_WINDOW_SHOWN
  );
  if (window == nullptr) {
    std::cerr << "WINDOW NULL.\n";
    return 1;
  }
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "CREATE RENDERER FAILURE\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }


  bool quit = false;
  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  std::cout << "Quitting.\n";
  return 0;
}
