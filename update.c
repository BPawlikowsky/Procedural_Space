#include "include/ProcSpace.h"

void updateTileCirclesPos(starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], double delta, int arrMax) {
  for(int x = 0; x < arrMax; x++) {
    for(int y =0; y < arrMax; y++) {
      if(systems[x][y].starSize != 0) {
        for(int p = 0; p < systems[x][y].planetCount; p++) {
          circle* c = &(systems[x][y].planets[p].c);
          updateCircle(c, delta, systems[x][y].sun);
        }
      }
    }
  }
}

void updateArrDims(float zoom, int* arrDim) {
  if(zoom < 0.9f) {
    switch((int)((1.0f-zoom)*10.0f)) {
    case 1: *arrDim = 2; break;
    case 2:
    case 3: *arrDim = 2; break;
    case 4:
    case 5: *arrDim = 2; break;
    case 6:
    case 7: *arrDim = 2; break;
    case 8:
    case 9: *arrDim = 3; break;
    }
  }
  else {
    *arrDim = 2;
  }
}

void updateTiles(ivec2d tPos,starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax) {
  int arrMin = 0;
  int arrHalf = (int)((arrMax-1)/2);
  for(int x = 0; x < arrMax; x++) {
    for(int y =0; y < arrMax; y++) {
      ivec2d vTemp = tPos;

      int tempx = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, x);
      int tempy = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, y);
      vTemp.x += tempx;
      vTemp.y += tempy;
      systems[x][y] = generateTile(vTemp, map, systems[x][y].texture);
    }
  }
}

void updateSize(int arrMax, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], float zoom) {
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax; y++) {
      systems[x][y].rect.w = (int)(TEX_SIZE * zoom);
      systems[x][y].rect.h = (int)(TEX_SIZE * zoom);
    }
}

void updateTilePos(ship player,starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], ivec2d tPos, float zoom, int arrMax) {
  int width = TEX_SIZE;
  int height = TEX_SIZE;
  ivec2d vtemp = tPos;
  int px = (player.pos.x - (vtemp.x * width))*zoom;
  int py = (player.pos.y - (vtemp.y * height))*zoom;
  int halfWidth = (int)(width/2);
  int halfHeight = (int)(height/2);

  int arrMin = 0;
  int arrHalf = (int)((arrMax-1)/2);
  int zero = intRange(-arrHalf, arrHalf, arrMin, arrMax-1, 0);
  if(zero >= 0) {
    systems[zero][zero].rect.x = width - px - ((TEX_SIZE - SCREEN_WIDTH) / 2);
    systems[zero][zero].rect.y = height - py - ((TEX_SIZE - SCREEN_HEIGHT) / 2);
    systems[zero][zero].rect.x = intRange(0, width, -halfWidth, halfWidth, systems[zero][zero].rect.x);
    systems[zero][zero].rect.y = intRange(0, height, -halfHeight, halfHeight, systems[zero][zero].rect.y);
  }

  for(int x = 0; x < arrMax; x++) {
    for(int y = 0; y < arrMax; y++) {
      int tempx = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, x);
      int tempy = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, y);
      if(zero >= 0) {
        systems[x][y].rect.x = systems[zero][zero].rect.x + (systems[zero][zero].rect.w * tempx);
        systems[x][y].rect.y = systems[zero][zero].rect.y + (systems[zero][zero].rect.h * tempy);
      }
    }
  }
}


void updateCircle(circle* c, double delta, circle sun) {
  gravity(c, sun);
  updateVelocity(c, delta);
}

void updateVelocity(circle* c, double delta) {
  c->vel.x += c->acc.x;
  c->vel.y += c->acc.y;
  c->pos.x += c->vel.x*delta;
  c->pos.y += c->vel.y*delta;
  c->acc.x = 0;
  c->acc.y = 0;
}

int updateTPos(ship player, ivec2d* tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax) {
  int pX = (int)(player.pos.x / TEX_SIZE);
  int pY = (int)(player.pos.y / TEX_SIZE);
  if(tPos->x != pX || tPos->y != pY) {

    if(tPos->x < pX)
      shiftTilesLeft(*tPos, systems, map, arrMax);
    if(tPos->x > pX)
      shiftTilesRight(*tPos, systems, map, arrMax);
    if(tPos->y < pY)
      shiftTilesUp(*tPos, systems, map, arrMax);
    if(tPos->y > pY)
      shiftTilesDown(*tPos, systems, map, arrMax);

    tPos->x = (int)(player.pos.x / TEX_SIZE);
    tPos->y = (int)(player.pos.y / TEX_SIZE);
    return 1;
  }
  return 0;
}

void updateSystem(int x, int y,starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, ivec2d tTemp) {
  systems[x][y] = generateTile(tTemp, map, systems[x][y].texture);
}

void updateControls(ship* player, int accel, float delta) {
  const Uint8* state = SDL_GetKeyboardState(NULL);
  float velMag = fvecMagnatudeGet(player->vel);

  if(state[SDL_SCANCODE_UP] && velMag < nTopSpeed) {
    player->acc.y -= accel;
  }
  else if(state[SDL_SCANCODE_DOWN] && velMag < nTopSpeed) {
    player->acc.y += accel;
  }
  else if(state[SDL_SCANCODE_LEFT] && velMag < nTopSpeed) {
    player->acc.x -= accel;
  }
  else if(state[SDL_SCANCODE_RIGHT] && velMag < nTopSpeed) {
    player->acc.x += accel;
  }
  else if(player->vel.x != 0 || player->vel.y != 0) {
      player->acc.x -= floorf(player->vel.x*0.01);
      player->acc.y -= floorf(player->vel.y*0.01);
  }

  player->vel.x += player->acc.x;
  player->vel.y += player->acc.y;

  player->pos.x += player->vel.x * delta;
  player->pos.y += player->vel.y * delta;

  //Edge check
  if(player->pos.x < 0)
    player->pos.x = 2;
  if(player->pos.y < 0)
    player->pos.y = 2;
  if(player->pos.x > (TEX_SIZE*TEX_SIZE))
    player->pos.x = (TEX_SIZE*TEX_SIZE)-2;
  if(player->pos.y > (TEX_SIZE*TEX_SIZE))
    player->pos.y = (TEX_SIZE*TEX_SIZE)-2;

  player->acc.x = 0;
  player->acc.y = 0;
}

void updateTextures(starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int arrMax) {
  //Prepaire Textures
  for(int x = 0; x < arrMax; x++) {
    for(int y =0; y < arrMax; y++) {
      renderTile(systems[x][y]);
    }
  }
}

void shiftTilesLeft(ivec2d tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax) {
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax; y++) {
      if(x == arrMax-1) {
        ivec2d tTemp = tPos;
        int arrMin = 0;
        int arrHalf = (int)((arrMax-1)/2);
        int tempx = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, x);
        int tempy = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, y);
        tTemp.x += tempx+1;
        tTemp.y += tempy;
        updateSystem(x, y, systems, map, tTemp);
      }
      else {
        systems[x][y] = systems[x+1][y];
      }
    }
}

void shiftTilesRight(ivec2d tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax) {
  for(int x = arrMax-1; x >= 0; x--)
    for(int y = arrMax-1; y >= 0; y--) {
      if(x == 0) {
        ivec2d tTemp = tPos;
        int arrMin = 0;
        int arrHalf = (int)((arrMax-1)/2);
        int tempx = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, x);
        int tempy = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, y);
        tTemp.x += tempx - 1;
        tTemp.y += tempy;
        updateSystem(x, y, systems, map, tTemp);
      }
      else {
        systems[x][y] = systems[x-1][y];
      }
    }
}

void shiftTilesUp(ivec2d tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax) {
  for(int x = 0; x < arrMax; x++)
    for(int y = 0; y < arrMax; y++) {
      if(y == arrMax-1) {
        ivec2d tTemp = tPos;
        int arrMin = 0;
        int arrHalf = (int)((arrMax-1)/2);
        int tempx = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, x);
        int tempy = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, y);
        tTemp.x += tempx;
        tTemp.y += tempy+1;
        updateSystem(x, y, systems, map, tTemp);
      }
      else {
        systems[x][y] = systems[x][y+1];
      }
    }
}

void shiftTilesDown(ivec2d tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax) {
  for(int x = arrMax-1; x >= 0; x--)
    for(int y = arrMax-1; y >= 0; y--) {
      if(y == 0) {
        ivec2d tTemp = tPos;
        int arrMin = 0;
        int arrHalf = (int)((arrMax-1)/2);
        int tempx = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, x);
        int tempy = intRange(arrMin, arrMax-1, -arrHalf, arrHalf, y);
        tTemp.x += tempx;
        tTemp.y += tempy - 1;
        updateSystem(x, y, systems, map, tTemp);
      }
      else {
        systems[x][y] = systems[x][y-1];
      }
    }
}

void updateShipAngle(ship* p) {
  fvec2d p1 = {p->points[0].x, p->points[0].y};
  fvec2d p2 = {p->points[0].x + p->vel.x, p->points[0].y + p->vel.y};
  static double lastang;

  double angle = atan2(p2.x - p1.x, p1.y - p2.y);
  SDL_bool cw = SDL_TRUE;
  if(angle < lastang)
    {
      cw = SDL_FALSE;
    }
  lastang = angle;

  p1 = fvecNormalize(p1);
  p2 = fvecNormalize(p2);
  double dp = fDotProduct(p1, p2);
  angle = acos(dp);

  for( int i = 0; i < 6; i++) {
    float x = p->points[i].x;
    float y = p->points[i].y;
    float px = 0;
    float py = 0;
    if (cw){
      px = (x * cos(angle) ) - (y * sin(angle) );
      py = (x * sin(angle) ) + (y * cos(angle) );
    }
    else{
      px = (x * cos(angle) ) + (y * sin(angle) );
      py = (x * -sin(angle) ) + (y * cos(angle) );
    }

    int danger = 100;
    if(px < danger && py < danger && px > -danger && py > -danger)
      {
        p->points[i].x = px;
        p->pointsd[i].x = px + (SCREEN_WIDTH/2);
        p->points[i].y = py;
        p->pointsd[i].y = py + (SCREEN_HEIGHT/2);
      }
    else
      {
        px = p->points[i].x;
        p->pointsd[i].x = px + (SCREEN_WIDTH/2);
        py = p->points[i].y;
        p->pointsd[i].y = py + (SCREEN_HEIGHT/2);
      }
  }
}

void updatePlayerGrav(ship* player, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE],int arrMax, double delta) {
  for(int x = 0; x < arrMax; x++) {
    for(int y = 0; y < arrMax; y++) {
      if(systems[x][y].starSize != 0) {
        circle c;
        c.pos.x = player->pos.x - (systems[x][y].posOnMap.x * TEX_SIZE);
        c.pos.y = player->pos.y - (systems[x][y].posOnMap.y * TEX_SIZE);
        c.acc.x = c.acc.y = c.vel.x = c.vel.y = 0;
        c.mass = 10;
        circle* cp = &c;
        gravity(cp, systems[x][y].sun);
        player->acc.x += c.acc.x * 0.1;
        player->acc.y += c.acc.y * 0.1;
        player->vel.x += player->acc.x;
        player->vel.y += player->acc.y;
        for(int p = 0; p < systems[x][y].planetCount; p++){
          c.pos.x = player->pos.x - (systems[x][y].posOnMap.x * TEX_SIZE);
          c.pos.y = player->pos.y - (systems[x][y].posOnMap.y * TEX_SIZE);
          c.acc.x = c.acc.y = c.vel.x = c.vel.y = 0;
          c.mass = 10;
          cp = &c;
          gravity(cp, systems[x][y].planets[p].c);
          player->acc.x += c.acc.x;
          player->acc.y += c.acc.y;
          player->vel.x += player->acc.x;
          player->vel.y += player->acc.y;

        }
      }
    }
  }
  player->pos.x += player->vel.x*delta;
  player->pos.y += player->vel.y*delta;
}
