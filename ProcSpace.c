#include "ProcSpace.h"

void init(void);
void render();
void update(float delta);
void updateTiles(SDL_bool dir);
void updateControls(float delta);
void updateTextures();

#define nScreenWidth 800
#define nScreenHeight 600

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture* firsttexture = NULL;
SDL_Texture* texture = NULL;
SDL_Texture* lasttexture = NULL;
SDL_Rect firstrect, rect, lastrect;
Uint32 nState = 0;
int map[nScreenWidth*nScreenHeight];
int tile[nScreenWidth*nScreenHeight];
starSystem firstSys, renderSys, lastSys;

vec2 tPos = {0,0};
int frames;
float milis;
SDL_bool pressed = SDL_FALSE;
ship player;
int speed = 5000;

int main(void) {
  // Init Video
  init();
  //Init Vars
  player.pos.x = (int)( nScreenWidth/2 );
  player.pos.y = (int)( nScreenHeight/2 );

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
          printf("rect x: %d y: %d | \n", rect.x, rect.y);
          printf("lastrect x: %d y: %d | \n", lastrect.x, lastrect.y);
          milis = 0.0f;
          frames = 0;
          pressed = SDL_FALSE;
        }
      }// End of main loop
  }

  //CLEANUP
  SDL_DestroyTexture(firsttexture);
  SDL_DestroyTexture(texture);
  SDL_DestroyTexture(lasttexture);

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
  firstrect.w = rect.w = lastrect.w = nScreenWidth;
  firstrect.h = rect.h = lastrect.h = nScreenHeight;
  firstrect.x = 0 - rect.w;
  rect.x = 0;
  lastrect.x = 0 + rect.w;
  firstrect.y = rect.y = lastrect.y = 0;
  //Initialize textures to SDL_TEXTUREACCESS_TARGET for drawing to them
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);
  lasttexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);
  firsttexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, nScreenWidth, nScreenHeight);
  //Generate "galaxy" map from nState seed
  generateMap(map);
}

void render() {
  //Clear screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  //Render Tiles
  SDL_RenderCopy(renderer, firsttexture, NULL, &firstrect);
  SDL_RenderCopy(renderer, texture, NULL, &rect);
  SDL_RenderCopy(renderer, lasttexture, NULL, &lastrect);

  //Render Player
  renderShip(renderer);

  SDL_RenderPresent(renderer);
}

void update(float delta) {
  updateControls(delta);


  if(lastrect.x <= 0) {
    updateTiles(SDL_TRUE);
    updateTextures();
    }

  if(firstrect.x >= 0) {
    updateTiles(SDL_FALSE);
    updateTextures();
  }
}

void updateTiles(SDL_bool dir) {
  if(dir)
    tPos.x++;
  else
    tPos.x--;
  vec2 vTemp = tPos;
  vTemp.x--;
  firstSys = generateTile(vTemp, nState, map);
  vTemp.x++;
  renderSys = generateTile(vTemp, nState, map);
  vTemp.x++;
  lastSys = generateTile(vTemp, nState, map);
  rect.x = nScreenWidth - (player.pos.x - ((tPos.x )* nScreenWidth));
  lastrect.x = rect.x + rect.w;
  firstrect.x = rect.x - rect.w;
}

void updateTextures() {
  //Prepaire Textures
  renderTile( renderSys, texture, renderer, nState);
  renderTile( lastSys, lasttexture, renderer, nState);
  renderTile( firstSys, firsttexture, renderer, nState);
}

void updateControls(float delta) {
  const Uint8* state = SDL_GetKeyboardState(NULL);

  if(state[SDL_SCANCODE_UP]) {
      player.pos.y -= (int)(speed*delta);
      if(player.pos.y <= 0)
        player.pos.y += (int)(speed*delta);
  }
  else if(state[SDL_SCANCODE_DOWN]) {
      player.pos.y += (int)(speed*delta);
      if(player.pos.y >= (nScreenHeight*nScreenHeight))
        player.pos.y += (int)(speed*delta);
  }
  else if(state[SDL_SCANCODE_LEFT]) {
      player.pos.x -= (int)(speed*delta);
      //Move Tiles
      firstrect.x +=  (int)(speed*delta);
      rect.x +=  (int)(speed*delta);
      lastrect.x +=  (int)(speed*delta);
      if(player.pos.x <= (int)(nScreenWidth/2))
        {
          player.pos.x += (int)(speed*delta);
          //Move Tiles
          firstrect.x -=  (int)(speed*delta);
          rect.x -=  (int)(speed*delta);
          lastrect.x -=  (int)(speed*delta);
        }
  }
  else if(state[SDL_SCANCODE_RIGHT]) {
      player.pos.x += (int)(speed*delta);
      //Move Tiles
      firstrect.x -=  (int)(speed*delta);
      rect.x -=  (int)(speed*delta);
      lastrect.x -=  (int)(speed*delta);
      if(player.pos.x >= (nScreenWidth*nScreenWidth)-(int)(nScreenWidth/2))
        {
          player.pos.x -= (int)(speed*delta);
          //Move Tiles
          firstrect.x +=  (int)(speed*delta);
          rect.x += (int)(speed*delta);
          lastrect.x += (int)(speed*delta);
        }
  }
}
