
//Updates
void updateArrDims(float zoom, int* arrDim);
void updateSystem(int x, int y,starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, ivec2d tTemp);
void updateSize(int arrMax, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], float zoom);
int updateTPos(ship player, ivec2d* tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax);
void updateTilePos(ship player,starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], ivec2d tPos, float zoom, int arrMax);
void updateTileCirclesPos(starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], double delta, int arrMax);
void updateTiles(ivec2d tPos,starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax);
void updateCircle(circle* c, double delta, circle sun);
void updateVelocity(circle* c, double delta);
void updateTexture(int x, int y);
void updateTextures(starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int arrMax);
void updateShipAngle(ship* p);
void updatePlayerGrav(ship* player, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE],int arrMax, double delta);
//updates-controls
void updateControls(ship* player, int accel, float delta);
void shiftTilesLeft(ivec2d tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax);
void shiftTilesRight(ivec2d tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax);
void shiftTilesUp(ivec2d tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax);
void shiftTilesDown(ivec2d tPos, starSystem systems[STAR_MAP_SIZE][STAR_MAP_SIZE], int* map, int arrMax);
