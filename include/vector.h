#include <math.h>

typedef struct {
  int x;
  int y;
} ivec2d;
typedef struct {
  float x;
  float y;
} fvec2d;

int intRange(int oldMin, int oldMax, int newMin, int newMax, int value);
float floatRange(float oldMin, float oldMax, float newMin, float newMax, float value);
int ivecDistance(ivec2d vec1, ivec2d vec2);
float fvecDistance(fvec2d vec1, fvec2d vec2);
ivec2d ivecSubstract(ivec2d vec1, ivec2d vec2);
fvec2d fvecSubstract(fvec2d vec1, fvec2d vec2);
int ivecMagnatudeGet(ivec2d vec);
double fvecMagnatudeGet(fvec2d vec);
ivec2d ivecMagnatudeSet(ivec2d vec, int mag);
fvec2d fvecMagnatudeSet(fvec2d vec, float mag);
ivec2d ivecNormalize(ivec2d vec);
fvec2d fvecNormalize(fvec2d vec);
float constrainf(float x, float min, float max);
double fDotProduct(fvec2d vec1, fvec2d vec2);
