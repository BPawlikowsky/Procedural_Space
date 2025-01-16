#pragma once
#include <SDL2/SDL.h>
#include "datatypes.h"
#include "config.h"

//Render
void renderMap(int* map, SDL_Renderer* renderer);
void renderOutline(starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int arrMax, SDL_Renderer* renderer);
void renderTiles(starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int arrMax, SDL_Renderer* renderer);
void renderShip(SDL_Renderer* renderer, SDL_FPoint* points);
void renderTile(starSystem sSystem);
void clearTexture(SDL_Texture* tex, SDL_Color color);
void drawCircle(circle c, SDL_Color color, void* pixels, int pitch);
