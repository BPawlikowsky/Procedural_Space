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

#define nScreenWidth 800
#define nScreenHeight 600
#define nTopSpeed 500
#define nMaxZoom 4

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture* textures[3][3];
SDL_Rect rects[3][3];
starSystem systems[3][3], currentSystem;
Uint32 nState = 0;
int map[nScreenWidth*nScreenHeight];

vec2 tPos = {0,0};
int frames;
float milis;
SDL_bool pressed = SDL_FALSE;
ship player;
int speed = 0;
float zoom = 4.0f;

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
          printf("zoom: %f\n", zoom);
          milis = 0.0f;
          frames = 0;
          pressed = SDL_FALSE;
        }
      }// End of main loop
  }

  //CLEANUP
  for(int rows = 0; rows < 3; rows++) {
      SDL_DestroyTexture(textures[rows][0]);
      SDL_DestroyTexture(textures[rows][1]);
      SDL_DestroyTexture(textures[rows][2]);
    }

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
  int y = -1;
  for(int rows = 0; rows < 3; rows++)
    {
      rects[rows][0].w = rects[rows][1].w = rects[rows][2].w = nScreenWidth;
      rects[rows][0].h = rects[rows][1].h = rects[rows][2].h = nScreenHeight;
      //Initialize textures to SDL_TEXTUREACCESS_TARGET for drawing to them
      textures[rows][0] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);
      textures[rows][1] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);
      textures[rows][2] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);
      y++;
    }
  //Generate "galaxy" map from nState seed
  generateMap(map);
}

void render() {
  //Clear screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  //Render Tiles
  for(int rows = 0; rows < 3; rows++) {
    SDL_RenderCopy(renderer, textures[rows][0], NULL, &rects[rows][0]);
    SDL_RenderCopy(renderer, textures[rows][1], NULL, &rects[rows][1]);
    SDL_RenderCopy(renderer, textures[rows][2], NULL, &rects[rows][2]);
    }

  //Render Player
  renderShip(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  for(int x = 0; x < 3; x++)
   for(int y = 0; y < 3;y++)
     SDL_RenderDrawRect(renderer, &rects[x][y]);

  SDL_RenderPresent(renderer);
}

void update(float delta) {
  updateControls(delta);
  updateTPos();
  updateSize();
  updateTilePos();
  updateTiles();
  updateTextures();
}

void updateTiles() {
    vec2 vTemp = tPos;
    vTemp.x--;
    vTemp.y--;
    for(int row = 0; row < 3; row++) {
      systems[row][0] = generateTile(vTemp, nState, map);
      vTemp.x++;
      systems[row][1] = currentSystem = generateTile(vTemp, nState, map);
      vTemp.x++;
      systems[row][2] = generateTile(vTemp, nState, map);
      vTemp.y++;
      vTemp.x-=2;
    }
}

void updateSize() {
  for(int x = 0; x < 3; x++)
    for(int y = 0; y < 3; y++) {
      rects[x][y].w = (int)(nScreenWidth );
      rects[x][y].h = (int)(nScreenHeight );
    }
}

void updateTilePos() {
  int px = (player.pos.x- (tPos.x * nScreenWidth));
  int py = (player.pos.y- (tPos.y * nScreenHeight));
  rects[1][1].x = (nScreenWidth - px);
  rects[1][1].y = (nScreenHeight - py);
  //Converting from [0 - ScreenSize] to [-1/2*ScreenSize - 1/2*ScreenSize]
  int nr = (nScreenWidth/2)-(-(nScreenWidth/2));
  rects[1][1].x = ((rects[1][1].x * nr) / nScreenWidth) + (-(nScreenWidth/2));
  nr = (nScreenHeight/2)-(-(nScreenHeight/2));
  rects[1][1].y = ((rects[1][1].y * nr) / nScreenHeight) + (-(nScreenHeight/2));

  rects[0][0].x = rects[1][1].x - rects[1][1].w;
  rects[0][1].x = rects[1][1].x;
  rects[0][2].x = rects[1][1].x + rects[1][1].w;
  rects[0][0].y = rects[1][1].y - rects[1][1].h;
  rects[0][1].y = rects[1][1].y - rects[1][1].h;
  rects[0][2].y = rects[1][1].y - rects[1][1].h;

  rects[1][0].x = rects[1][1].x - rects[1][1].w;
  rects[1][2].x = rects[1][1].x + rects[1][1].w;
  rects[1][0].y = rects[1][1].y;
  rects[1][2].y = rects[1][1].y;

  rects[2][0].x = rects[1][1].x - rects[1][1].w;
  rects[2][1].x = rects[1][1].x;
  rects[2][2].x = rects[1][1].x + rects[1][1].w;
  rects[2][0].y = rects[1][1].y + rects[1][1].h;
  rects[2][1].y = rects[1][1].y + rects[1][1].h;
  rects[2][2].y = rects[1][1].y + rects[1][1].h;
}

void updateTextures() {
  //Prepaire Textures
  for(int rows = 0; rows < 3; rows++) {
    renderTile(systems[rows][0], textures[rows][0], renderer, nState);
    renderTile(systems[rows][1], textures[rows][1], renderer, nState);
    renderTile(systems[rows][2], textures[rows][2], renderer, nState);
  }
}

void updateControls(float delta) {
  const Uint8* state = SDL_GetKeyboardState(NULL);

  if(state[SDL_SCANCODE_UP]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    player.pos.y -= (int)(speed*delta);

    if(player.pos.y <= (int)(nScreenHeight/2))
      player.pos.y += (int)(speed*delta);
  }

  else if(state[SDL_SCANCODE_DOWN]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    player.pos.y += (int)(speed*delta);
    if(player.pos.y >= (nScreenHeight*nScreenHeight)-(int)(nScreenHeight/2))
      player.pos.y -= (int)(speed*delta);
  }

  else if(state[SDL_SCANCODE_LEFT]) {
    if(speed < nTopSpeed)
      speed ++;
    zoom = zoomCalc();
    player.pos.x -= (int)(speed*delta);

    if(player.pos.x <= (int)(nScreenWidth/2))
      player.pos.x += (int)(speed*delta);
  }
  else if(state[SDL_SCANCODE_RIGHT]) {
    if(speed < nTopSpeed) speed ++;
    zoom = zoomCalc();
    player.pos.x += (int)(speed*delta);

    if(player.pos.x >= (nScreenWidth*nScreenWidth)-(int)(nScreenWidth/2))
      {
        player.pos.x -= (int)(speed*delta);
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
  float zoomSpeed = speed * (0.2f - nMaxZoom) / (nTopSpeed) + nMaxZoom;
  return zoomSpeed;
}

void updateTPos() {
  tPos.x = (int)(player.pos.x / nScreenWidth);
  tPos.y = (int)(player.pos.y / nScreenHeight);
}
