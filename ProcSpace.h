#include <SDL2/SDL.h>
#include <math.h>
#define nScreenWidth 800
#define nScreenHeight 600

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
  SDL_Texture* texture;
  SDL_Rect rect;
} starSystem;

int generateRand(int nMin, int nMax, uint32_t nState);
uint32_t Lehmer32(uint32_t state);
void generateMap(int* map);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void changeColor(SDL_Surface* surface, Uint32 color);
void renderMap(int* map, SDL_Renderer* renderer);
starSystem generateTile(vec2 tilePos,Uint32 nState, int* map);
starSystem generateSystem(vec2 pos,Uint32 nState,char starType, int starSize, int planetCount, int planetSize);
void rederShip();
void renderTile(starSystem sSystem, SDL_Renderer* renderer, Uint32 nState);
float floatRange(float val, float oldMax, float oldMin, float newMax, float newMin);
int intRange(int val, int oldMax, int oldMin, int newMax, int newMin);

uint32_t Lehmer32(uint32_t state) {
  uint64_t product;
  state += 0xe120fc15;
	product = (uint64_t)state * 0x4a39b70d;

  uint32_t m1 = (product >> 32) ^ product;
  product = (uint64_t)m1 * 0x12fad5c9;
  uint32_t m2 = (product >> 32) ^ product;

	return m2;
}

int generateRand(int nMin, int nMax, uint32_t nState) {
  return (int)(Lehmer32(nState) % (nMax - nMin)) + nMin;
}

void generateMap(int* map) {
  //Generate map
  for(int x = 0; x < nScreenWidth; x++) {
    for(int y = 0; y < nScreenHeight; y++) {
      SDL_bool bIsStar = SDL_FALSE;

      int nSeed = y << 16 | x;

      uint32_t nState = nSeed;

      bIsStar = Lehmer32(nState) % 256 < 6;

      if(bIsStar) {
        SDL_bool bIsBlueGiant = Lehmer32(nState) % 256 < 1;
        SDL_bool bIsGreenGiant = Lehmer32(nState) % 256 < 2;
        SDL_bool bIsRedGiant = Lehmer32(nState) % 256 < 3;
        if(bIsRedGiant) {
          map[x + (y*nScreenHeight)] = 'R';
          if(bIsGreenGiant) {
            map[x + (y*nScreenHeight)] = 'G';
            if(bIsBlueGiant) {
              map[x + (y*nScreenHeight)] = 'B';
            }
          }
        }
        else
          map[x + (y*nScreenHeight)] = 'W';
      }
      else
        map[x + (y*nScreenHeight)] = 'S';
    }
  }
}

void renderMap(int* map, SDL_Renderer* renderer) {
  //Render Map
  for(int x = 0; x < nScreenWidth; x++) {
    for(int y = 0; y < nScreenHeight; y++) {
      switch(map[x + (nScreenHeight*y)]) {
      case 'S': {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      }break;
      case 'R': {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
      }break;
      case 'G': {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
      }break;
      case 'B': {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
      }break;
      case 'W': {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
      }break;
      }
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }
}

void changeColor(SDL_Surface* surface, Uint32 color) {
  for(int x = 0; x < 160; x++) {
    for(int y = 0; y < 160; y++) {
      Uint32 pix = getpixel(surface, x, y);
      if((pix != 0x00000000))
        putpixel(surface, x, y, color);
    }
  }
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
    return *p;
    break;

  case 2:
    return *(Uint16 *)p;
    break;

  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
    break;

  case 4:
    return *(Uint32 *)p;
    break;

  default:
    return 0;       /* shouldn't happen, but avoids warnings */
  }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(Uint16 *)p = pixel;
    break;

  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;

  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
}

starSystem generateTile(vec2 tilePos, Uint32 nState, int* map) {
  switch(map[tilePos.x + (nScreenHeight*tilePos.y)]) {
  case 'S': {
  }break;
  case 'R': {
    return generateSystem(tilePos, nState,'R', 10, 3, 3);
  }break;
  case 'G': {
    return generateSystem(tilePos, nState,'G', 7, 5, 4);
  }break;
  case 'B': {
    return generateSystem(tilePos, nState,'B', 5, 7, 5);
  }break;
  case 'W': {
    return generateSystem(tilePos, nState,'W', 4, 10, 6);
  }break;
  }
  starSystem s;
  s.starSize = 0;
  return s;
}

starSystem generateSystem(vec2 pos,Uint32 nState,char starType, int starSize, int planetCount, int planetSize) {
  int nSeed = pos.y << 16 | pos.x;

  nState = nSeed;

  starSystem tileSys;
  tileSys.posOnMap.x = pos.x;
  tileSys.posOnMap.y = pos.y;
  printf("generateSystem: pos.x: %d y: %d\n", pos.x, pos.y);
  tileSys.starType = starType;
  tileSys.starSize = generateRand(starSize-3, starSize, nState);
  tileSys.planetCount = generateRand(1, planetCount, nState);
  for(int i = 0; i < tileSys.planetCount; i++) {
    planet tempPlanet;
    tempPlanet.size = generateRand(1, planetSize, nState+i);
    int l = (generateRand(starSize*10, (int)(nScreenWidth/2)-planetSize*10, nState+i));
    int x = l*(cos(l))  + nScreenWidth/2;
    tempPlanet.pos.x = x;
    l = (generateRand(20, nScreenHeight/2, nState+i));
    int y = l*(sin(l))  + (int)( nScreenHeight/2 )-(planetSize*10);
    tempPlanet.pos.y = y;
    tempPlanet.type = generateRand(1, 3, nState) + 'A';
    tileSys.planets[i] = tempPlanet;
  }
  puts("generateSystem: Finished");
  return tileSys;
}

void renderTile(starSystem sSystem, SDL_Renderer* renderer, Uint32 nState) {

  if(sSystem.texture == NULL)
    {
      puts("Render Tile: texture Null");
    }
  puts("renderTile: Before SetRenderTarget");
  void* mPixels = NULL;
  int mPitch;
  SDL_LockTexture(sSystem.texture, NULL, &mPixels, &mPitch);
  SDL_SetRenderTarget(renderer, sSystem.texture);

  puts("renderTile: After SetRenderTarget");
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  if(sSystem.starSize != 0) {
    puts("renderTile: Before temp Texture and Surface create");
    SDL_Surface* tempSurf = SDL_LoadBMP("star.bmp");
    SDL_Texture* tempTex = SDL_CreateTextureFromSurface(renderer, tempSurf);
    puts("renderTile: After temp Texture and Surface create");
    Uint32 color;
    switch(sSystem.starType) {
    case 'R': { color = 0xffff0000;} break;
    case 'G': { color = 0xff00ff00;} break;
    case 'B': { color = 0xff0000ff;} break;
    case 'W': { color = 0xffffffff;} break;
    }
    changeColor(tempSurf, color);
    SDL_UpdateTexture(tempTex, NULL, tempSurf->pixels, tempSurf->pitch);

    SDL_Rect tempRect;
    int renderSize = sSystem.starSize * 16;
    tempRect.x = (int)(nScreenWidth / 2) - (int)(renderSize / 2);
    tempRect.y = (int)(nScreenHeight / 2) - (int)(renderSize / 2);
    tempRect.w = renderSize;
    tempRect.h = renderSize;
    SDL_RenderCopy(renderer, tempTex, NULL, &tempRect);
    for(int i = 0; i < sSystem.planetCount; i++) {
      renderSize = sSystem.planets[i].size * 16;
      switch(sSystem.planets[i].type) {
      case 'A': { color = 0xff000000 | generateRand(0x22222222, 0x00ffffff, nState+i);} break;
      case 'B': { color = 0xff000000 | generateRand(0x22222222, 0x00ffffff, nState+i+1);} break;
      case 'C': { color = 0xff000000 | generateRand(0x22222222, 0x00ffffff, nState+i+2);} break;
      case 'D': { color = 0xff000000 | generateRand(0x22222222, 0x00ffffff, nState+i+3);} break;
      }
      changeColor(tempSurf, color);
      SDL_UpdateTexture(tempTex, NULL, tempSurf->pixels, tempSurf->pitch);
      tempRect.x = sSystem.planets[i].pos.x;
      tempRect.y = sSystem.planets[i].pos.y;
      tempRect.w = renderSize;
      tempRect.h = renderSize;
      SDL_RenderCopy(renderer, tempTex, NULL, &tempRect);
    }
    SDL_FreeSurface(tempSurf);
    SDL_DestroyTexture(tempTex);
  }
  SDL_SetRenderTarget(renderer, NULL);
  SDL_UnlockTexture(sSystem.texture);
  free(mPixels);
}

void renderShip(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawPoint(renderer,
                      (int)(nScreenWidth/2),
                      (int)(nScreenHeight/2)
                      );
}


int intRange(int val, int oldMax, int oldMin, int newMax, int newMin) {
  int oldRange = oldMax - oldMin;
  int newRange = newMax - newMin;
  return (int)(((val - oldMin) * newRange) / oldRange) + newMin;
}

float floatRange(float val, float oldMax, float oldMin, float newMax, float newMin) {
  float oldRange = oldMax - oldMin;
  float newRange = newMax - newMin;
  return (((val - oldMin) * newRange) / oldRange) + newMin;
}
