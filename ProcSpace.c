#include "ProcSpace.h"

vec2 pos = {0,0};


int accel = 2;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
starSystem systems[15][15];
Uint32 nState = 0;
int map[nScreenWidth*nScreenHeight];

vec2 tPos = {0,0};
int frames;
float milis;
SDL_bool pressed = SDL_FALSE;
ship player;
float speed = 0;
float zoom = 4.0f;
int arrDim = 1;
int arrMax = 15;

int main(void) {
  // Init Video
  init();
  //Init Vars
  player.pos.x = (nScreenWidth*nScreenWidth)/2;
  player.pos.y = (nScreenHeight*nScreenHeight)/2;
  updateTPos();

  updateTiles();
  updateSize();
  updateTilePos();
  updateTextures(systems, arrMax, renderer, nState);

  //Time tracking
  uint32_t lastTick, currentTick;
  lastTick = SDL_GetTicks();
  frames = 0;
  milis = 0.0f;

  //If window created, continue
  if(window != NULL) {
    SDL_bool done = SDL_FALSE;

    //Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    //Render "galaxy" map and wait for a sec
    renderMap(map, renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);

    //Main Loop
    while(!done)
      {
        SDL_Event event;
        double fElapsedTime;
        fElapsedTime = (double)(currentTick - lastTick)/1000.0f;

        lastTick = currentTick;

        //UPDATE AND RENDER
        update(fElapsedTime);
        render();

        while(SDL_PollEvent(&event))
          if(event.type == SDL_QUIT)
            done = SDL_TRUE;

        //Elapsed Time counting and debug stuff
        currentTick = SDL_GetTicks();
        milis += fElapsedTime;
        frames ++;
        if(milis >= 1.0f) {
          printf("tilePos x: %d y: %d | m: %f\n", tPos.x, tPos.y, milis);
          printf("PPos x: %d y: %d | \n", player.pos.x, player.pos.y);
          printf("zoom: %f | arrDim: %d \n", zoom, arrDim);
          printf("%d\n",frames);
          showTileInfo();
          milis = 0.0f;
          frames = 0;
          pressed = SDL_FALSE;
        }
      }// End of main loop
  }

  //CLEANUP
  for(int x = 0; x < 15; x++)
    for(int y = 0; y < 15; y++) 
      SDL_DestroyTexture(systems[x][y].texture);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  puts("Cleaned up!");
}

//Init SDL and all render objects
void init() {
  //SDL Init
  SDL_Init(SDL_INIT_VIDEO);
  //Create window and renderer
  SDL_CreateWindowAndRenderer(nScreenWidth, nScreenHeight, 0, &window, &renderer);
  //Initialize render tiles positions
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax;y++)
    {
      systems[x][y].rect.w = nScreenWidth;
      systems[x][y].rect.h = nScreenHeight;
      //Initialize textures to SDL_TEXTUREACCESS_TARGET for drawing to them
      systems[x][y].texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);
    }
  //Generate "galaxy" map from nState seed
  generateMap(map);
}

void render() {
  //Clear screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  renderTiles(systems, arrMax, renderer);

  renderOutline(systems, arrMax, renderer);

  //Render Player
  renderShip(renderer);

  SDL_RenderPresent(renderer);
}

void update(float delta) {
  updateControls(delta);
  updateArrDims();
  arrMax = (2*arrDim)-1;
  updateTPos();

  //updateTiles();
  updateSize();
  updateTilePos();
  //updateTextures();
}

void updateArrDims() {
  if(zoom < 0.9f) {
    switch((int)((1.0f-zoom)*10.0f)) {
    case 1: arrDim = 2; break;
    case 2:
    case 3: arrDim = 2; break;
    case 4:
    case 5: arrDim = 2; break;
    case 6:
    case 7: arrDim = 2; break;
    case 8:
    case 9: arrDim = 8; break;
    }
  }
  else {
    arrDim = 2;
  }
}

void updateTiles() {
  int arrMin = 0;
  int arrHalf = (int)((arrMax-1)/2);
  for(int x = 0; x < arrMax; x++) {
    for(int y =0; y < arrMax; y++) {
      vec2 vTemp = tPos;

      int tempx = intRange(x, arrMax-1, arrMin, arrHalf, -arrHalf);
      int tempy = intRange(y, arrMax-1, arrMin, arrHalf, -arrHalf);
      vTemp.x += tempx;
      vTemp.y += tempy;
      if(systems[x][y].texture != NULL) {
        //SDL_DestroyTexture(systems[x][y].texture);
      }
      systems[x][y] = generateTile(vTemp, nState, map);
    }
  }
}

void updateSize() {
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax; y++) {
      systems[x][y].rect.w = (int)(nScreenWidth * zoom);
      systems[x][y].rect.h = (int)(nScreenHeight * zoom);
    }
}

void updateTilePos() {
  int width = nScreenWidth;
  int height = nScreenHeight;
  vec2 vtemp = tPos;
  int px = (player.pos.x - (vtemp.x * width))*zoom;
  int py = (player.pos.y - (vtemp.y * height))*zoom;
  int halfWidth = (int)(width/2);
  int halfHeight = (int)(height/2);

  int arrMin = 0;
  int arrHalf = (int)((arrMax-1)/2);
  int zero = intRange(0, arrHalf, -arrHalf, arrMax-1, arrMin);
  if(zero >= 0) {
  systems[zero][zero].rect.x = width - px;
  systems[zero][zero].rect.y = height - py;
  systems[zero][zero].rect.x = intRange(systems[zero][zero].rect.x, width, 0, halfWidth, -halfWidth);
  systems[zero][zero].rect.y = intRange(systems[zero][zero].rect.y, height, 0, halfHeight, -halfHeight);
  }

  for(int x = 0; x < arrMax; x++) {
    for(int y = 0; y < arrMax; y++) {
      int tempx = intRange(x, arrMax-1, arrMin, arrHalf, -arrHalf);
      int tempy = intRange(y, arrMax-1, arrMin, arrHalf, -arrHalf);
      if(zero >= 0) {
        systems[x][y].rect.x = systems[zero][zero].rect.x + (systems[zero][zero].rect.w * tempx);
        systems[x][y].rect.y = systems[zero][zero].rect.y + (systems[zero][zero].rect.h * tempy);
      }
    }
  }
}


void updateControls(float delta) {
  const Uint8* state = SDL_GetKeyboardState(NULL);

  if(state[SDL_SCANCODE_UP]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    pos.y --;

    if(player.pos.y <= (int)(nScreenHeight/2))
      pos.y ++;
  }
  else if(state[SDL_SCANCODE_DOWN]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    pos.y ++;
    if(player.pos.y >= (nScreenHeight*nScreenHeight)-(int)(nScreenHeight/2))
      pos.y --;
  }
  else if(state[SDL_SCANCODE_LEFT]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    pos.x --;

    if(player.pos.x <= (int)(nScreenWidth/2))
      pos.x ++;
  }
  else if(state[SDL_SCANCODE_RIGHT]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    pos.x ++;

    if(player.pos.x >= (nScreenWidth*nScreenWidth)-(int)(nScreenWidth/2))
        pos.x --;
  }
  else {
    if(speed > 0) {
      speed -= (1/speed)*delta;
      zoom = zoomCalc();
    }
    else {
      speed = 0;
      zoom = nMaxZoom;
    }
  }
  player.pos.x += (int)(pos.x*delta);
  player.pos.y += (int)(pos.y*delta);
}

float zoomCalc() {
  return floatRange(speed, nTopSpeed, 0, nMinZoom, nMaxZoom);
}

void shiftTilesLeft() {
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax; y++) {
      if(x == arrMax-1) {
        vec2 tTemp = tPos;
        int arrMin = 0;
        int arrHalf = (int)((arrMax-1)/2);
        int tempx = intRange(x, arrMax-1, arrMin, arrHalf, -arrHalf);
        int tempy = intRange(y, arrMax-1, arrMin, arrHalf, -arrHalf);
        tTemp.x += tempx+1;
        tTemp.y += tempy;
        updateSystem(x, y, tTemp);
        updateTexture(x, y);
      }
      else {
        systems[x][y] = systems[x+1][y];
      }
    }
}

void shiftTilesRight() {
  for(int x = 0; x < arrMax; x++)
    for(int y = arrMax-1; y >= 0; y--) {
      if(x == 0) {
        vec2 tTemp = tPos;
        int arrMin = 0;
        int arrHalf = (int)((arrMax-1)/2);
        int tempx = intRange(x, arrMax-1, arrMin, arrHalf, -arrHalf);
        int tempy = intRange(y, arrMax-1, arrMin, arrHalf, -arrHalf);
        tTemp.x += tempx - 1;
        tTemp.y += tempy;
        updateSystem(x, y, tTemp);
        updateTexture(x, y);
      }
      else {
        systems[x][y] = systems[x-1][y];
      }
    }
}

void showTileInfo() {
  for(int y = 0; y < arrMax; y++) {
    for(int x = 0; x < arrMax; x++) {
    
      printf("%d ", systems[x][y].starSize);
    }
    printf("\n");
  }
  puts("---------------------------------------------");
}

void updateTPos() {
  int pX = (int)(player.pos.x / nScreenWidth);
  int pY = (int)(player.pos.y / nScreenHeight);
  if(tPos.x != pX || tPos.y != pY) {

    if(tPos.x < pX)
      shiftTilesLeft();
    if(tPos.x > pX)
      shiftTilesRight();

    tPos.x = (int)(player.pos.x / nScreenWidth);
    tPos.y = (int)(player.pos.y / nScreenHeight);
    // updateTiles();
    // updateTextures();
  }
}

void updateSystem(int x, int y, vec2 tTemp) {
  systems[x][y] = generateTile(tTemp, nState, map);
}

void updateTexture(int x, int y) {
  if(systems[x][y].texture != NULL) {
    //SDL_DestroyTexture(systems[x][y].texture);
  }
  systems[x][y].texture = renderTile(systems[x][y], renderer, nState);
}

void updateTextures(starSystem systems[15][15], int arrMax, SDL_Renderer* renderer, Uint32 nState) {
  //Prepaire Textures
  for(int x = 0; x < arrMax; x++) {
    for(int y =0; y < arrMax; y++) {
      if(systems[x][y].texture != NULL) {
        //SDL_DestroyTexture(systems[x][y].texture);
      }
      systems[x][y].texture = renderTile(systems[x][y], renderer, nState);
    }
  }
}
