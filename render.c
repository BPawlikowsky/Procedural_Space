#include "include/ProcSpace.h"

void renderTiles(starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int arrMax, SDL_Renderer* renderer) {
  SDL_Color clear = {0, 0, 0, 0};
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax;y++) {
      clearTexture(systems[x][y].texture, clear);
      renderTile(systems[x][y]);
      SDL_RenderCopy(renderer, systems[x][y].texture, NULL, &systems[x][y].rect);
    }
}

void renderOutline(starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int arrMax, SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax;y++)
      SDL_RenderDrawRect(renderer, &systems[x][y].rect);
}

void renderMap(int* map, SDL_Renderer* renderer) {
  //Render Map
  for(int x = 0; x < SCREEN_WIDTH; x++) {
    for(int y = 0; y < SCREEN_HEIGHT; y++) {
      switch(map[x + (SCREEN_HEIGHT*y)]) {
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

void renderTile(starSystem sSystem) {
  if(sSystem.starSize != 0){
    void* pixels;
    int pitch;
    if( SDL_LockTexture(sSystem.texture, NULL, &pixels, &pitch) == 0) {
      drawCircle(sSystem.sun, sSystem.sun.color, pixels, pitch);
      for(int i = 0; i < sSystem.planetCount; i++) {
        drawCircle(sSystem.planets[i].c, sSystem.planets[i].c.color, pixels, pitch);
      }
      SDL_UnlockTexture(sSystem.texture);
    }
    else
      printf("[renderTile][%d][%d] Failed to lock texture: %s\n", sSystem.posOnMap.x, sSystem.posOnMap.y, SDL_GetError());
  }
}

void renderShip(SDL_Renderer* renderer, SDL_FPoint* points) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLinesF(renderer, points, 6);
}

void clearTexture(SDL_Texture* tex, SDL_Color color) {
  void* pixels;
  int pitch;
  if( SDL_LockTexture(tex, NULL, &pixels, &pitch) == 0) {
    for(int i = 0; i < TEX_SIZE; i++) {
      Uint32* dst = (Uint32*)((Uint8*)pixels + i*pitch);
      for(int j = 0; j < TEX_SIZE; j++){
        //printf("[%d][%d] %d\n", i, j, *dst); 
        *dst++ =  ((color.r<<16) | (color.g<<8) | (color.b))<<8 | (color.a);
      }
    }

    SDL_UnlockTexture(tex);
  }
  else
    printf("[Clear Texture] Failed to lock texture: %s\n", SDL_GetError());
}

void drawCircle(circle c, SDL_Color color, void* pixels, int pitch) {
  for(int fill = 0; fill < c.r; fill++)
    for(float t = 0.0; t < 2*3.142; t+=1.0/c.r) {
      int x = c.pos.x;
      int y = c.pos.y;
      x += fill*cos(t);
      y += fill*sin(t);

      if(x < TEX_SIZE && x > 0 && y < TEX_SIZE && y > 0)
        {
          Uint32* dst = (Uint32*)((Uint8*)pixels + y*pitch);
          dst += x;
          *dst =  ((color.r<<16) | (color.g<<8) | (color.b))<<8 | (color.a);
        }
    }
}
