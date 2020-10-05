#include "include/ProcSpace.h"
#include <time.h>
ivec2d pos = {(14*TEX_SIZE)+250,1*TEX_SIZE};
int accel = 200;

SDL_Rect r;
SDL_Color background = {0, 0, 0, 255};
SDL_Color sunColor = {255, 255, 0, 255};
SDL_Color grey = {100, 100, 100, 255};
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
ivec2d tPos = {0,0};
char* title = "Procedural Space";
uint32_t lastTick, currentTick;
ship player;
float zoom = 0.4f;
int arrDim = 2;
int arrMax = STAR_MAP_SIZE;

starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE];
int map[TEX_SIZE * TEX_SIZE];

circle sun = {
  {(TEX_SIZE/2), (TEX_SIZE/2)},
  30,
  5000,
  0,
  {0, 0},
  {0, 0},
  {255, 255, 0, 255}
};

int main(void) {


  // Init Video
  if(init()) {
    return 1;
  }

  if(initStarMap(map)) {
    puts("Error generating map");
    return 1;
  }

  r.x = (TEX_SIZE/2) - (SCREEN_WIDTH/2);
  r.y = (TEX_SIZE/2) - (SCREEN_HEIGHT/2);
  r.w = SCREEN_WIDTH;
  r.h = SCREEN_HEIGHT;

  if(window != NULL)
    {
      //Time tracking
      currentTick = lastTick = SDL_GetTicks();
      int frames = 0;
      double milis = 0.0f;

      initPlayer();
      initSystems();

      SDL_bool done = SDL_FALSE;
      while(!done)
        {
          SDL_Event event;

          double delta;
          delta = (double)((currentTick - lastTick)*0.00001f);

          lastTick = currentTick;


          update(delta);
          if(frames % 2 == 0) {
            render();
          }

          //Time tracking
          currentTick = SDL_GetTicks();
          milis += delta*100;
          frames ++;
          if(milis >= 1.0f) {
            showTileInfo();
            printf("fps: %d | tPos x: %d y: %d | arrMax: %d\n", frames, tPos.x, tPos.y, arrMax);
            printf("player x: %f y: %f vel: %f\n", player.pos.x, player.pos.y, fvecMagnatudeGet(player.vel));

            milis = 0.0f;
            frames = 0;
          }

          //Event polling
          while(SDL_PollEvent(&event))
            if(event.type == SDL_QUIT)
              done = SDL_TRUE;
        }
    }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

int init() {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Error initializing SDL! %s\n", SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow(title,
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH,
                            SCREEN_HEIGHT,
                            0);
  if(!window) {
    printf("Error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  Uint32 renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  renderer = SDL_CreateRenderer(window, -1, renderFlags);
  if(!renderer) {
    printf("Error creating renderer: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  return 0;
}

int initStarMap(int* map) {

  //Initialize render tiles positions
  for(int x = 0; x < STAR_MAP_SIZE; x++)
    for(int y = 0; y < STAR_MAP_SIZE;y++)
      {
        systems[x][y].rect.w = TEX_SIZE;
        systems[x][y].rect.h = TEX_SIZE;
        systems[x][y].texture = SDL_CreateTexture(renderer,
                                                  SDL_PIXELFORMAT_RGBA8888,
                                                  SDL_TEXTUREACCESS_STREAMING,
                                                  TEX_SIZE,
                                                  TEX_SIZE
                                                  );
        if(systems[x][y].texture == NULL) {
          printf("[%d][%d] Failed to create texture: %s\n", x, y, SDL_GetError());
          return 1;
        }
        if(SDL_SetTextureBlendMode(systems[x][y].texture, SDL_BLENDMODE_BLEND)) {
          printf("[%d][%d] Failed to create texture: %s\n", x, y, SDL_GetError());
          return 1;
        }
      }
  //Generate "galaxy" map from nState seed
  generateMap(map);
  return 0;
}

void initPlayer() {

  player.pos.x = pos.x;
  player.pos.y = pos.y;
  player.shipSize = 15.0;

  player.points[0].x = (player.shipSize * zoom);
  player.points[0].y = 0;
  player.points[1].x = - (player.shipSize * 0.5 * zoom);
  player.points[1].y = (player.shipSize * 0.5 * zoom);
  player.points[2].x = - (player.shipSize * 0.5 * zoom);
  player.points[2].y = (player.shipSize * 0.5 * zoom);
  player.points[3].x = - (player.shipSize * 0.5 * zoom);
  player.points[3].y = - (player.shipSize * 0.5 * zoom);
  player.points[4].x = - (player.shipSize * 0.5 * zoom);
  player.points[4].y = - (player.shipSize * 0.5 * zoom);
  player.points[5].x = (player.shipSize * zoom);
  player.points[5].y = 0;

}

void initSystems() {
  tPos.x = (int)(player.pos.x / TEX_SIZE);
  tPos.y = (int)(player.pos.y / TEX_SIZE);
  updateArrDims(zoom, &arrDim);
  arrMax = (2*arrDim)-1;
  updateSize(arrMax, systems, zoom);
  updateTilePos(player, systems, tPos, zoom, arrMax);
  updateTiles(tPos, systems, map, arrMax);
}


void update(double delta) {
  updatePlayerGrav(&player, systems, arrMax, delta);
  updateControls(&player, accel, delta);
  updateShipAngle(&player);

  updateArrDims(zoom, &arrDim);
  arrMax = (2*arrDim)-1;
  zoom = zoomCalc();
  updateTPos(player, &tPos, systems, map, arrMax);

  updateSize(arrMax, systems, zoom);
  updateTilePos(player, systems, tPos, zoom, arrMax);

  updateTileCirclesPos(systems, delta, arrMax);
}


void render() {
  //Clear screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  renderTiles(systems, arrMax, renderer);

  renderOutline(systems, arrMax, renderer);

  //Render Player
  renderShip(renderer, player.pointsd);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  float scale = 0.007;
  SDL_RenderDrawPoint(renderer, (player.vel.x*scale*zoom) + 400, (player.vel.y*scale*zoom) + 300);
  //Render buffer
  SDL_RenderPresent(renderer);
}

float zoomCalc() {
  return floatRange(0,  100, nMaxZoom, nMinZoom, (player.vel.x > player.vel.y) ? player.vel.x : player.vel.y);
}

void showTileInfo() {
  for(int y = 0; y < arrMax; y++) {
    for(int x = 0; x < arrMax; x++) {
      printf("%c ", systems[x][y].starType);
    }
    printf("\n");
  }
  puts("---------------------------------------------");
}
