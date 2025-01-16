#include "include/utils.h"

int generateRand(int nMin, int nMax, uint32_t nState) {
  return (int)(Lehmer32(nState) % (nMax - nMin)) + nMin;
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

void gravity(circle* c, circle sun) {
  fvec2d force = fvecSubstract( sun.pos, c->pos);
  double mag = fvecMagnatudeGet(force);
  float distance = constrainf(mag*mag, 25.0, 10000.0);
  double strength = G * (sun.mass * c->mass) / distance;
  force = fvecMagnatudeSet(fvecNormalize(force), strength);

  force.x /= c->mass;
  force.y /= c->mass;
  c->acc.x += force.x;
  c->acc.y += force.y;
}
