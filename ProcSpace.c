#include <SDL2/SDL.h>
#include "ProcSpace.h"

void init(void);
void update(float delta, vec2* tilePos);
void generateMap();
starSystem* generateTile( vec2* tilePos);
void generateSpace();
starSystem* generateRed(vec2* pos);
void generateGreen();
void generateBlue();
void generateWhite();
void renderMap();
void renderTile(starSystem* sSystem);

#define nScreenWidth 512
#define nScreenHeight 512

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t nState = 0;
int map[nScreenWidth*nScreenHeight];
int tile[nScreenWidth*nScreenHeight];
starSystem* renderSys;

vec2 tPos = {0,0};
int frames, fps;
float milis;

int main(void) {
  // Init Video
  init();

  //Time tracking
  uint32_t lastTick, currentTick;
  lastTick = SDL_GetTicks();
  frames = 0;
  milis = 0.0f;


  if(window != NULL) {
    SDL_bool done = SDL_FALSE;

    //Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    renderMap();
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
    renderSys = malloc(sizeof(starSystem));

    //Main Loop
    while(!done)
      {
        double fElapsedTime;
        puts("B\n");
        fElapsedTime = (double)(currentTick - lastTick)/1000.0f;
        puts("A\n");
        SDL_Event event;

        lastTick = currentTick;

        update(fElapsedTime, &tPos);

        while(SDL_PollEvent(&event))
          if(event.type == SDL_QUIT)
            done = SDL_TRUE;

        currentTick = SDL_GetTicks();
        milis += fElapsedTime;
        frames ++;
        printf("f: %d | m: %f\n", frames, milis);
        if(milis >= 1.0f) {
          milis = 0.0f;
          frames = 0;
        }
      }
  }

  free(renderSys);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void init() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(nScreenWidth, nScreenHeight, 0, &window, &renderer);
  generateMap();
}

void update(float delta, vec2* tilePos) {
  const char* state = SDL_GetKeyboardState(NULL);

  //Clear screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  renderSys = generateTile(tilePos);
  renderTile( renderSys);

  SDL_RenderPresent(renderer);
  if(frames % 100 == 0) tPos.x++;
  printf("%d %f\n", tPos.x, delta);
}


starSystem* generateTile(vec2* tilePos) {
  switch(map[tilePos->x + (nScreenHeight*tilePos->y)]) {
  case 'S': {
    generateSpace();
  }break;
  case 'R': {
    return generateRed(tilePos);
  }break;
  case 'G': {
    return generateRed(tilePos);

    generateGreen();
  }break;
  case 'B': {
    return generateRed(tilePos);
 
    generateBlue();
  }break;
  case 'W': {
    return generateRed(tilePos);

    generateWhite();
  }break;
  }
  starSystem* s;
  s->starSize = 0;
  return s;
}

void generateMap() {
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

void generateSpace() {
  for(int x = 0; x < nScreenWidth; x++) {
    for(int y = 0; y < nScreenHeight; y++) {
      tile[x + (y*nScreenHeight)] = 'S';
    }
  }
}

starSystem* generateRed(vec2* pos){
  for(int x = 0; x < nScreenWidth; x++) {
    for(int y = 0; y < nScreenHeight; y++) {
      tile[x + (y*nScreenHeight)] = 'S';
    }
    puts("stage 0");
    int nSeed = pos->y << 16 | pos->x;

    puts("stage 1");
    uint32_t nState = nSeed;

    starSystem* tileSys;
    tileSys = malloc(sizeof(starSystem));
    //tileSys->posOnMap.x = pos->x;
    puts("stage 2");
    //tileSys->posOnMap.y = pos->y;
    tileSys->starType = 'R';
    tileSys->starSize = generateRand(7, 10, nState);
    puts("stage 3");
    tileSys->planetCount = generateRand(1,3, nState);
    puts("stage 4");
    for(int i = 0; i < tileSys->planetCount; i++) {
      puts("stage 5");
      planet* tempPlanet;
      tempPlanet->pos.x = generateRand(0,  nScreenWidth, nState);
      tempPlanet->pos.y = generateRand(0,  nScreenHeight, nState);
      tempPlanet->size = generateRand(1, 5, nState);
      tempPlanet->type = generateRand(1, 3, nState) + 'A';
      puts("stage 6");
      tileSys->planets[i] = tempPlanet;
    }
    printf("System generated!\n");
    return tileSys;

  }
  tile[(int)(nScreenWidth/2) + (int)(nScreenHeight*(nScreenHeight)/2)] = 'R';
}

void generateGreen() {
  for(int x = 0; x < nScreenWidth; x++) {
    for(int y = 0; y < nScreenHeight; y++) {
      tile[x + (y*nScreenHeight)] = 'S';
    }
  }
  tile[(int)(nScreenWidth/2) + (int)((nScreenHeight)/2)] = 'G';
}

void generateBlue() {
  for(int x = 0; x < nScreenWidth; x++) {
    for(int y = 0; y < nScreenHeight; y++) {
      tile[x + (y*nScreenHeight)] = 'S';
    }
  }
  tile[(int)(nScreenWidth/2) + (int)((nScreenHeight)/2)] = 'B';
}

void generateWhite() {
  for(int x = 0; x < nScreenWidth; x++) {
    for(int y = 0; y < nScreenHeight; y++) {
      tile[x + (y*nScreenHeight)] = 'S';
    }
  }
  tile[(int)(nScreenWidth/2) + (int)((nScreenHeight)/2)] = 'W';
}

void renderMap() {
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

void renderTile(starSystem* sSystem) {
  if(sSystem->starSize != 0) {
    puts("Rendering");
    SDL_Surface* surface = SDL_LoadBMP("star.bmp");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    rect.x = (int)(nScreenWidth / 2);
    rect.y = (int)(nScreenHeight / 2);
    rect.w = 160;
    rect.h = 160;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    puts(SDL_GetError());
  }
}

