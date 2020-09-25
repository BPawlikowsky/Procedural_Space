#include "ProcSpace.h"

void renderTiles(starSystem systems[15][15], int arrMax, SDL_Renderer* renderer) {
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax;y++)
      SDL_RenderCopy(renderer, systems[x][y].texture, NULL, &systems[x][y].rect);
}

void renderOutline(starSystem systems[15][15], int arrMax, SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax;y++)
      SDL_RenderDrawRect(renderer, &systems[x][y].rect);
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

SDL_Texture* renderTile(starSystem sSystem, SDL_Renderer* renderer, Uint32 nState) {
  SDL_Texture* canves = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);

  if(canves == NULL)
    {
      puts("Render Tile: texture Null");
    }

  SDL_SetRenderTarget(renderer, canves);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  if(sSystem.starSize != 0) {
    SDL_Surface* tempSurf = SDL_LoadBMP("star.bmp");
    SDL_Texture* tempTex = SDL_CreateTextureFromSurface(renderer, tempSurf);
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
  return canves;
}

void renderShip(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawPoint(renderer,
                      (int)(nScreenWidth/2),
                      (int)(nScreenHeight/2)
                      );
}
