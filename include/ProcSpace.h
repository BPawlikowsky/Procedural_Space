/*  ProcSpace.h - the main include file containing other includes and data types */
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include "utils.h"
#include "generate.h"
#include "render.h"
#include <time.h>

//Main
int init(void);
int initStarMap(int* map);
void initPlayer();
void initSystems();
void update(double delta);
void render();
void showTileInfo();
float zoomCalc();
