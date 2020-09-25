#include "ProcSpace.h"

int generateRand(int nMin, int nMax, uint32_t nState) {
  return (int)(Lehmer32(nState) % (nMax - nMin)) + nMin;
}

int intRange(int val, int oldMax, int oldMin, int newMax, int newMin) {
  int oldRange = oldMax - oldMin;
  int newRange = newMax - newMin;
  return (int)(((val - oldMin) * newRange) / oldRange) + newMin;
}

float floatRange(float val, float oldMax, float oldMin, float newMax, float newMin) {
  float oldRange = oldMax - oldMin;
  float newRange = newMax - newMin;
  return (((val - oldMin) * newRange) / oldRange) + newMin;
}

uint32_t Lehmer32(uint32_t state) {
  uint64_t product;
  state += 0xe120fc15;
	product = (uint64_t)state * 0x4a39b70d;

  uint32_t m1 = (product >> 32) ^ product;
  product = (uint64_t)m1 * 0x12fad5c9;
  uint32_t m2 = (product >> 32) ^ product;

	return m2;
}