#include "include/vector.h"

int intRange(int oldMin, int oldMax, int newMin, int newMax, int value) {
  int oldRange = oldMax - oldMin;
  int newRange = newMax - newMin;
  return (((value-oldMin)*newRange)/oldRange) + newMin;
}

float floatRange(float oldMin, float oldMax, float newMin, float newMax, float value) {
  float oldRange = oldMax - oldMin;
  float newRange = newMax - newMin;
  return (((value-oldMin)*newRange)/oldRange) + newMin;
}

int ivecDistance(ivec2d vec1, ivec2d vec2) {
  return (int)(pow((vec2.x - vec1.x), 2) + pow((vec2.y - vec1.y), 2));
}

float fvecDistance(fvec2d vec1, fvec2d vec2) {
  return (float)(pow((vec2.x - vec1.x), 2.0) + pow((vec2.y - vec1.y), 2.0));
}

ivec2d ivecSubstract(ivec2d vec1, ivec2d vec2) {
  ivec2d v = {(vec1.x - vec2.x), (vec1.y - vec2.y)};
  return v;
}

fvec2d fvecSubstract(fvec2d vec1, fvec2d vec2) {
  fvec2d v = {(vec1.x - vec2.x), (vec1.y - vec2.y)};
  return v;
}

int ivecMagnatudeGet(ivec2d vec) {
  return (int)sqrt((vec.x*vec.x) + (vec.y*vec.y));
}

double fvecMagnatudeGet(fvec2d vec) {
  return sqrt((vec.x*vec.x) + (vec.y*vec.y));
}

ivec2d ivecMagnatudeSet(ivec2d vec, int mag) {
  ivec2d v = {(vec.x*mag), (vec.y*mag)};
  return v;
}

fvec2d fvecMagnatudeSet(fvec2d vec, float mag) {
  fvec2d v = {(vec.x*mag), (vec.y*mag)};
  return v;
}

fvec2d fvecNormalize(fvec2d vec) {
  double term = 1.0/sqrt(vec.x*vec.x + vec.y*vec.y);
  fvec2d v = {vec.x*term, vec.y*term};
  return v;
}

float constrainf(float x, float min, float max) {
  if(x > min && x < max)
    return x;
  else if(x > max)
    return max;
  else
    return min;
}

double fDotProduct(fvec2d vec1, fvec2d vec2) {
  return (vec1.x*vec2.x) + (vec1.y*vec2.y);
}
