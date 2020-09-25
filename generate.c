#include "ProcSpace.h"

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
  s.rect.w = nScreenWidth;
  s.rect.h = nScreenHeight;
  s.starType = 'S';
  return s;
}

starSystem generateSystem(vec2 pos,Uint32 nState,char starType, int starSize, int planetCount, int planetSize) {
  int nSeed = pos.y << 16 | pos.x;

  nState = nSeed;

  starSystem tileSys;
  tileSys.posOnMap.x = pos.x;
  tileSys.posOnMap.y = pos.y;
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
  tileSys.rect.w = nScreenWidth;
  tileSys.rect.h = nScreenHeight;
  return tileSys;
}