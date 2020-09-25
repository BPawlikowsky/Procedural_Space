#include <SDL2/SDL.h>
#include <math.h>
#define nScreenWidth 800
#define nScreenHeight 600
#define nTopSpeed 550
#define nMaxZoom 0.07f
#define nMinZoom 0.07f

typedef struct {
  int x;
  int y;
} vec2;

typedef struct {
  float speed;
  float dir;
  vec2 pos;
} ship;

typedef struct {
  vec2 pos;
  int size;
  char type;
} planet;

typedef struct {
  vec2 posOnMap;
  char starType;
  int starSize;
  planet planets[10];
  int planetCount;
  SDL_Rect rect;
  SDL_Texture *texture;
} starSystem;

void init(void);
void renderMap(int* map, SDL_Renderer* renderer);
void generateMap(int* map);
void renderShip(SDL_Renderer* renderer);
void render();
void renderOutline(starSystem systems[15][15], int arrMax, SDL_Renderer* renderer);
void renderTiles(starSystem systems[15][15], int arrMax, SDL_Renderer* renderer);
void update(float delta);
void updateTiles();
void updateControls(float delta);
void updateTextures();
void updateTPos();
void updateTilePos();
void updateSize();
void updateTileContent();
void updateArrDims();
void updateEdgeTiles();
void updateSystem(int x, int y, vec2 tTemp);
void updateTexture(int x, int y);
void showTileInfo() ;
float zoomCalc();
int generateRand(int nMin, int nMax, uint32_t nState);
uint32_t Lehmer32(uint32_t state);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void changeColor(SDL_Surface* surface, Uint32 color);
starSystem generateTile(vec2 tilePos,Uint32 nState, int* map);
starSystem generateSystem(vec2 pos,Uint32 nState,char starType, int starSize, int planetCount, int planetSize);
SDL_Texture* renderTile(starSystem sSystem, SDL_Renderer* renderer, Uint32 nState);
float floatRange(float val, float oldMax, float oldMin, float newMax, float newMin);
int intRange(int val, int oldMax, int oldMin, int newMax, int newMin);
