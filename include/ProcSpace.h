/*  ProcSpace.h - the main include file containing other includes and data types */
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "vector.h"
#include "datatypes.h"
#define TEX_SIZE 1024
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define STAR_MAP_SIZE 15
#define G 40
#define nTopSpeed 100000
#define nMaxZoom 0.40f
#define nMinZoom 0.40f
#include "utils.h"
#include "generate.h"
#include "update.h"
#include "render.h"

//Main
int init(void);
int initStarMap(int* map);
void initPlayer();
void initSystems();
void update(double delta);
void render();
void showTileInfo();
float zoomCalc();
