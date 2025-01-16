#pragma once
#include "vector.h"
#include <SDL2/SDL.h>

typedef struct {
  fvec2d pos;
  int r;
  float mass;
  int orbit;
  fvec2d acc;
  fvec2d vel;
  SDL_Color color;
} circle;

typedef struct {
  float dir;
  fvec2d pos;
  fvec2d acc;
  fvec2d vel;
  float shipSize;
  SDL_FPoint points[6];
  SDL_FPoint pointsd[6];
} ship;

typedef struct {
  ivec2d pos;
  int size;
  char type;
  circle c;
} planet;

typedef struct {
  ivec2d posOnMap;
  char starType;
  int starSize;
  planet planets[10];
  int planetCount;
  SDL_Rect rect;
  SDL_Texture* texture;
  circle sun;
} starSystem;
