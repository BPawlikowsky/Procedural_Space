#include "ProcSpace.h"

void init(void);
void render();
void update(float delta);
void updateTiles();
void updateControls(float delta);
void updateTextures();
void updateTPos();
void updateTilePos();
void updateSize();
float zoomCalc();
void renderOutline();
void updateTileContent();
void updateArrDims();

#define nScreenWidth 800
#define nScreenHeight 600
#define nTopSpeed 550
#define nMaxZoom 5
#define nMinZoom 0.1f
int accel = 2;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture* textures[27][27];
SDL_Rect rects[27][27];
starSystem systems[27][27], currentSystem;
Uint32 nState = 0;
int map[nScreenWidth*nScreenHeight];

vec2 tPos = {0,0};
int frames;
float milis;
SDL_bool pressed = SDL_FALSE;
ship player;
int speed = 0;
float zoom = 4.0f;
int arrDim = 1;

int main(void) {
  // Init Video
  init();
  //Init Vars
  player.pos.x = (int)( (nScreenWidth*nScreenWidth) / 2);
  player.pos.y = (int)((nScreenHeight*nScreenHeight / 2));

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
          milis = 0.0f;
          frames = 0;
          pressed = SDL_FALSE;
        }
      }// End of main loop
  }

  //CLEANUP
  for(int x = 0; x < 27; x++)
    for(int y = 0; y < 27; y++) 
      SDL_DestroyTexture(textures[x][y]);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

//Init SDL and all render objects
void init() {
  //SDL Init
  SDL_Init(SDL_INIT_VIDEO);
  //Create window and renderer
  SDL_CreateWindowAndRenderer(nScreenWidth, nScreenHeight, 0, &window, &renderer);
  //Initialize render tiles positions
  int arrMax = 27;
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax;y++)
    {
      rects[x][y].w = rects[x][y].w = rects[x][y].w = nScreenWidth;
      rects[x][y].h = rects[x][y].h = rects[x][y].h = nScreenHeight;
      //Initialize textures to SDL_TEXTUREACCESS_TARGET for drawing to them
      textures[x][y] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);
    }
  //Generate "galaxy" map from nState seed
  generateMap(map);
}

void render() {
  //Clear screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  //Render Tiles
  int arrMax = (3*arrDim)-1;
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax;y++)
      SDL_RenderCopy(renderer, textures[x][y], NULL, &rects[x][y]);
  renderOutline();

  //Render Player
  renderShip(renderer);

  SDL_RenderPresent(renderer);
}

void renderOutline() {
  int arrMax = (3*arrDim)-1;
  SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax;y++)
      SDL_RenderDrawRect(renderer, &rects[x][y]);
}

void update(float delta) {
  updateControls(delta);
  updateTPos();

  //Switch controlling odd num in arrays
  updateArrDims();
  //updateTileContent();

  updateSize();
  updateTilePos();
}

void updateArrDims() {
  
  if(zoom < 0.9f) {
    switch((int)((1.0f-zoom)*10.0f)) {
    case 1: arrDim = 3; break;
    case 2:
    case 3: arrDim = 3; break;
    case 4:
    case 5: arrDim = 3; break;
    case 6:
    case 7: arrDim = 3; break;
    case 8:
    case 9: arrDim = 5; break;
    }
  }
  else {
    arrDim = 1;
    updateTiles();
    updateTextures();
  }
}

void updateTileContent() {
  int arrMax = (3*arrDim)-1;
  vec2 vTemp = tPos;
  vTemp.x += intRange(arrMax-1, arrMax-1, 0, (arrMax/2), -(arrMax/2));
  vTemp.y += intRange(arrMax-1, arrMax-1, 0, (arrMax/2), -(arrMax/2));
  starSystem sTemp = generateTile(vTemp, nState, map);
  if(zoom < 1.0f && systems[arrMax-1][arrMax-1].posOnMap.x != sTemp.posOnMap.x) {
    updateTiles();
    updateTextures();
  }

  vTemp.x += intRange(0, arrMax-1, 0, (arrMax/2), -(arrMax/2));
  vTemp.y += intRange(0, arrMax-1, 0, (arrMax/2), -(arrMax/2));
  sTemp = generateTile(vTemp, nState, map);
  if(zoom < 1.0f && systems[0][0].posOnMap.x != sTemp.posOnMap.x) {
    updateTiles();
    updateTextures();
  }
}

void updateTiles() {
  int arrMax = (3*arrDim)-1;
  int arrMin = 0;
  int arrHalf = (int)(arrMax/2);
  for(int x = 0; x < arrMax; x++) {
    for(int y =0; y < arrMax; y++) {
      vec2 vTemp = tPos;
      int tempx = intRange(x, arrMax, arrMin, arrHalf, -arrHalf);
      int tempy = intRange(y, arrMax, arrMin, arrHalf, -arrHalf);
      vTemp.x += tempx;
      vTemp.y += tempy;
      systems[x][y] = generateTile(vTemp, nState, map);
    }
  }
}

void updateSize() {
  int arrMax = (3*arrDim)-1;
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax; y++) {
      rects[x][y].w = (int)(nScreenWidth * zoom);
      rects[x][y].h = (int)(nScreenHeight * zoom);
    }
}

void updateTilePos() {
  int width = nScreenWidth;
  int height = nScreenHeight;
  int px = (player.pos.x - (tPos.x * width))*zoom;
  int py = (player.pos.y - (tPos.y * height))*zoom;
  int halfWidth = width/2;
  int halfHeight = height/2;

  int arrMax = (3*arrDim)-1;
  int arrMin = 0;
  int arrHalf = (int)(arrMax/2);
  int zero = intRange(0, arrHalf, -arrHalf, arrMax, arrMin);
  rects[zero][zero].x = ((width) - px);
  rects[zero][zero].y = ((height) - py);
  rects[zero][zero].x = intRange(rects[zero][zero].x, width, 0, halfWidth, -halfWidth);
  rects[zero][zero].y = intRange(rects[zero][zero].y, height, 0, halfHeight, -halfHeight);

  for(int x = 0; x < arrMax; x++) {
    for(int y =0; y < arrMax; y++) {
      int tempx = intRange(x, arrMax, arrMin, arrHalf, -arrHalf);
      int tempy = intRange(y, arrMax, arrMin, arrHalf, -arrHalf);
      int zero = intRange(0, arrHalf, -arrHalf, arrMax, arrMin);
      rects[x][y].x = rects[zero][zero].x + (rects[zero][zero].w * tempx);
      rects[x][y].y = rects[zero][zero].y + (rects[zero][zero].h * tempy);
    }
  }

}

void updateTextures() {
  //Prepaire Textures
  int arrMax = (3*arrDim)-1;
  for(int x = 0; x < arrMax; x++) {
    for(int y =0; y < arrMax; y++) {
      renderTile(systems[x][y], textures[x][y], renderer, nState);
    }
  }
}

void updateControls(float delta) {
  const Uint8* state = SDL_GetKeyboardState(NULL);

  if(state[SDL_SCANCODE_UP]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    player.pos.y -= (int)(speed*accel*delta);

    if(player.pos.y <= (int)(nScreenHeight/2))
      player.pos.y += (int)(speed*accel*delta);
  }
  else if(state[SDL_SCANCODE_DOWN]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    player.pos.y += (int)(speed*accel*delta);
    if(player.pos.y >= (nScreenHeight*nScreenHeight)-(int)(nScreenHeight/2))
      player.pos.y -= (int)(speed*accel*delta);
  }
  else if(state[SDL_SCANCODE_LEFT]) {
    if(speed < nTopSpeed)
      speed ++;
    zoom = zoomCalc();
    player.pos.x -= (int)(speed*accel*delta);

    if(player.pos.x <= (int)(nScreenWidth/2))
      player.pos.x += (int)(speed*accel*delta);
  }
  else if(state[SDL_SCANCODE_RIGHT]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    player.pos.x += (int)(speed*accel*delta);

    if(player.pos.x >= (nScreenWidth*nScreenWidth)-(int)(nScreenWidth/2))
      {
        player.pos.x -= (int)(speed*accel*delta);
      }
  }
  else {
    if(speed > 0) {
      speed --;
      zoom = zoomCalc();
    }
    else {
      speed = 0;
      zoom = nMaxZoom;
    }
  }
}

float zoomCalc() {
  return floatRange(speed, nTopSpeed, 0, nMinZoom, nMaxZoom);
}

void updateTPos() {
  if(tPos.x != (int)(player.pos.x / nScreenWidth) ||
                     tPos.y != (int)(player.pos.y / nScreenHeight)) {
    tPos.x = (int)(player.pos.x / nScreenWidth);
    tPos.y = (int)(player.pos.y / nScreenHeight);

    updateTiles();
    updateTextures();
  }
}
