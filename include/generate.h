#pragma once
#include "datatypes.h"
#include "vector.h"
#include "config.h"
#include "render.h"
#include "utils.h"
#include "render.h"
#include <SDL2/SDL.h>

//Generate
starSystem generateTile(ivec2d tilePos, int* map, SDL_Texture* tex);
starSystem generateSystem(ivec2d pos,char starType, int starSize, int planetCount, int planetSize, SDL_Texture* tex);
void generateMap(int* map);
void initCircle(circle* c, float mass, int orbit, circle sun);
float testOrbit(circle c, circle sun, double delta);
