#include "include/ProcSpace.h"

void generateMap(int* map) {
  //Generate map
  for(int x = 0; x < SCREEN_WIDTH; x++) {
    for(int y = 0; y < SCREEN_HEIGHT; y++) {
      SDL_bool bIsStar = SDL_FALSE;

      int nSeed = y << 16 | x;

      uint32_t nState = nSeed;

      bIsStar = Lehmer32(nState) % 256 < 6;

      if(bIsStar) {
        SDL_bool bIsBlueGiant = Lehmer32(nState) % 256 < 1;
        SDL_bool bIsGreenGiant = Lehmer32(nState) % 256 < 2;
        SDL_bool bIsRedGiant = Lehmer32(nState) % 256 < 3;
        if(bIsRedGiant) {
          map[x + (y*SCREEN_HEIGHT)] = 'R';
          if(bIsGreenGiant) {
            map[x + (y*SCREEN_HEIGHT)] = 'G';
            if(bIsBlueGiant) {
              map[x + (y*SCREEN_HEIGHT)] = 'B';
            }
          }
        }
        else
          map[x + (y*SCREEN_HEIGHT)] = 'W';
      }
      else
        map[x + (y*SCREEN_HEIGHT)] = 'S';
    }
  }
}


starSystem generateTile(ivec2d tilePos, int* map, SDL_Texture* tex) {
  SDL_Color clear = {0, 0, 0, 0};
  clearTexture(tex, clear);
  switch(map[tilePos.x + (SCREEN_HEIGHT*tilePos.y)]) {
  case 'S': {
  }break;
  case 'R': {
    return generateSystem(tilePos, 'R', 10, 3, 3, tex);
  }break;
  case 'G': {
    return generateSystem(tilePos, 'G', 7, 5, 4, tex);
  }break;
  case 'B': {
    return generateSystem(tilePos, 'B', 5, 7, 5, tex);
  }break;
  case 'W': {
    return generateSystem(tilePos, 'W', 4, 10, 6, tex);
  }break;
  }
  starSystem s;
  s.starSize = 0;
  s.rect.w = TEX_SIZE;
  s.rect.h = TEX_SIZE;
  s.starType = 'S';
  s.texture = tex;
  return s;
}

starSystem generateSystem(ivec2d pos,char starType, int starSize, int planetCount, int planetSize, SDL_Texture* tex) {
  int nSeed = pos.y << 16 | pos.x;

  Uint32 nState = nSeed;

  SDL_Color red = {255, 0, 0, 255};
  SDL_Color green = {0, 255, 0, 255};
  SDL_Color blue = {0, 0, 255, 255};
  SDL_Color white = {255, 255, 255, 255};
  SDL_Color color;
  switch(starType) {
  case 'R': color = red; break;
  case 'G': color = green; break;
  case 'B': color = blue; break;
  default: color = white; break;
  }

  starSystem tileSys;
  tileSys.posOnMap.x = pos.x;
  tileSys.posOnMap.y = pos.y;
  tileSys.starType = starType;
  tileSys.starSize = generateRand(starSize-3, starSize, nState);

  tileSys.sun.pos.x = TEX_SIZE/2;
  tileSys.sun.pos.y = TEX_SIZE/2;
  tileSys.sun.r = tileSys.starSize*10;
  tileSys.sun.mass = 100000 - (10000 * tileSys.starSize);
  tileSys.sun.acc.x = tileSys.sun.acc.y = 0;
  tileSys.sun.vel.x = tileSys.sun.vel.y = 0;
  tileSys.sun.orbit = 0;
  tileSys.sun.color = color;

  tileSys.planetCount = generateRand(1, planetCount, nState);
  for(int i = 0; i < tileSys.planetCount; i++) {
    planet tempPlanet;
    tempPlanet.size = generateRand(1, planetSize, nState+i);
    int l = (generateRand(75, (int)(TEX_SIZE/2)-planetSize*10, nState+i));
    l += i*10;
    int x = l*(cos(l)) + TEX_SIZE/2;
    int y = l*(sin(l))  + (int)( SCREEN_HEIGHT/2 )-(planetSize*10);
    tempPlanet.pos.x = x;
    tempPlanet.pos.y = y;
    tempPlanet.type = generateRand(1, 3, nState) + 'A';

    initCircle(&tempPlanet.c, tempPlanet.size*10, l, tileSys.sun);
    tempPlanet.c.mass = tempPlanet.size * 100;
    tempPlanet.c.color.r = generateRand(50, 255, nState+i*2);
    tempPlanet.c.color.g = generateRand(50, 255, nState+i*3);
    tempPlanet.c.color.b = generateRand(50, 255, nState+i);

    tileSys.planets[i] = tempPlanet;
  }
  tileSys.rect.w = TEX_SIZE;
  tileSys.rect.h = TEX_SIZE;
  tileSys.texture = tex;
  return tileSys;
}

void initCircle(circle* c, float mass, int orbit, circle sun) {
  int nSeed = (int)c->pos.y << 16 | (int)c->pos.x;

  Uint32 nState = nSeed;
  SDL_Color planetColor;

  c->mass = mass;
  c->r = c->mass/2;
  c->orbit = orbit;
  c->pos.x = TEX_SIZE/2;
  c->pos.y = (TEX_SIZE/2) - c->orbit;
  planetColor.r = generateRand(20, 255, nState);
  planetColor.g = generateRand(20, 255, nState);
  planetColor.b = generateRand(20, 255, nState);
  planetColor.a = 255;
  c->color = planetColor;
  c->vel.x = c->vel.y = c->acc.x = c->acc.y = 0;

  float constval = testOrbit(*c, sun, 0.000528);
  float escVel = c->orbit + (c->orbit * constval);
  fvec2d v = {1, 0};
  v.x = 1;
  v.y = 0;
  v = fvecMagnatudeSet(v, escVel);
  c->vel.x = v.x;

}

float testOrbit(circle c, circle sun, double delta) {
  SDL_bool done = SDL_FALSE;
  float val = 0.0;
  while(!done) {
    circle test = c;

    if(val > 30.0*G)
      done = SDL_TRUE;

    float escVel = test.orbit + (test.orbit * val);
    fvec2d v = {1, 0};
    v.x = 1;
    v.y = 0;
    v = fvecMagnatudeSet(v, escVel);
    test.vel.x = v.x;

    long int time = 1000*test.orbit;
    for(long int t = 0; t < time; t++) {
      updateCircle(&test, delta, sun);

      float dist = (fvecMagnatudeGet(fvecSubstract(sun.pos, test.pos)));
      float th = 10;
      if(dist > (TEX_SIZE / 2) || dist > (test.orbit + th) || dist < (test.orbit - th)) {
        val += 0.01;
        break;
      } else if( t > (time-10)) {
        return val;
      }
    }
  }
  return -1;
}
