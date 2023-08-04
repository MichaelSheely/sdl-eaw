#include <SDL.h>

// From https://github.com/rtrussell/BBCSDL/blob/master/src/SDL2_gfxPrimitives.c
// by Andreas Schiffler.
int pixel(SDL_Renderer* renderer, Sint16 x, Sint16 y) {
  return SDL_RenderDrawPoint(renderer, x, y);
}

// From https://github.com/rtrussell/BBCSDL/blob/master/src/SDL2_gfxPrimitives.c
// by Andreas Schiffler with some edits to how color and non-circles are handled.
int ellipseRGBA(
    SDL_Renderer* renderer, Sint16 x, Sint16 y,
    Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  int result;
  int ix, iy;
  int h, i, j, k;
  int oh, oi, oj, ok;
  int xmh, xph, ypk, ymk;
  int xmi, xpi, ymj, ypj;
  int xmj, xpj, ymi, ypi;
  int xmk, xpk, ymh, yph;

  /*
  * Sanity check radii
  */
  if ((rx < 0) || (ry < 0)) {
          return (-1);
  }

  /*
  * Special case for rx=0 - reject input
  */
  if (rx == 0) {

    return 1;
  }
  /*
  * Special case for ry=0 - reject input
  */
  if (ry == 0) {
    return 1;
  }

  /*
  * Set color
  */
  result = 0;
  // if (a != 255) {
  //   result |= SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  // }
  result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

  /*
  * Init vars
  */
  oh = oi = oj = ok = 0xFFFF;

  /*
  * Draw
  */
  if (rx > ry) {
          ix = 0;
          iy = rx * 64;

          do {
                  h = (ix + 32) >> 6;
                  i = (iy + 32) >> 6;
                  j = (h * ry) / rx;
                  k = (i * ry) / rx;

                  if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
                          xph = x + h;
                          xmh = x - h;
                          if (k > 0) {
                                  ypk = y + k;
                                  ymk = y - k;
                                  result |= pixel(renderer, xmh, ypk);
                                  result |= pixel(renderer, xph, ypk);
                                  result |= pixel(renderer, xmh, ymk);
                                  result |= pixel(renderer, xph, ymk);
                          } else {
                                  result |= pixel(renderer, xmh, y);
                                  result |= pixel(renderer, xph, y);
                          }
                          ok = k;
                          xpi = x + i;
                          xmi = x - i;
                          if (j > 0) {
                                  ypj = y + j;
                                  ymj = y - j;
                                  result |= pixel(renderer, xmi, ypj);
                                  result |= pixel(renderer, xpi, ypj);
                                  result |= pixel(renderer, xmi, ymj);
                                  result |= pixel(renderer, xpi, ymj);
                          } else {
                                  result |= pixel(renderer, xmi, y);
                                  result |= pixel(renderer, xpi, y);
                          }
                          oj = j;
                  }

                  ix = ix + iy / rx;
                  iy = iy - ix / rx;

          } while (i > h);
  } else {
          ix = 0;
          iy = ry * 64;

          do {
                  h = (ix + 32) >> 6;
                  i = (iy + 32) >> 6;
                  j = (h * rx) / ry;
                  k = (i * rx) / ry;

                  if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
                          xmj = x - j;
                          xpj = x + j;
                          if (i > 0) {
                                  ypi = y + i;
                                  ymi = y - i;
                                  result |= pixel(renderer, xmj, ypi);
                                  result |= pixel(renderer, xpj, ypi);
                                  result |= pixel(renderer, xmj, ymi);
                                  result |= pixel(renderer, xpj, ymi);
                          } else {
                                  result |= pixel(renderer, xmj, y);
                                  result |= pixel(renderer, xpj, y);
                          }
                          oi = i;
                          xmk = x - k;
                          xpk = x + k;
                          if (h > 0) {
                                  yph = y + h;
                                  ymh = y - h;
                                  result |= pixel(renderer, xmk, yph);
                                  result |= pixel(renderer, xpk, yph);
                                  result |= pixel(renderer, xmk, ymh);
                                  result |= pixel(renderer, xpk, ymh);
                          } else {
                                  result |= pixel(renderer, xmk, y);
                                  result |= pixel(renderer, xpk, y);
                          }
                          oh = h;
                  }

                  ix = ix + iy / ry;
                  iy = iy - ix / ry;

          } while (i > h);
  }

  return (result);
}
