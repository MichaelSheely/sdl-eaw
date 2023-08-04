#ifndef SDL_EAW_DRAW_CIRCLE_H_
#define SDL_EAW_DRAW_CIRCLE_H_
#include <SDL.h>

// From https://github.com/rtrussell/BBCSDL/blob/master/src/SDL2_gfxPrimitives.c
// by Andreas Schiffler.
int pixel(SDL_Renderer* renderer, Sint16 x, Sint16 y);

// From https://github.com/rtrussell/BBCSDL/blob/master/src/SDL2_gfxPrimitives.c
// by Andreas Schiffler with some edits to how color and non-circles are handled.
int ellipseRGBA(SDL_Renderer* renderer, Sint16 x, Sint16 y,
                Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif  // SDL_EAW_DRAW_CIRCLE_H_
