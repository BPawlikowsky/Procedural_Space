#pragma once
#include "datatypes.h"
#include "config.h"

//Utils
int intRange(int oldMin, int oldMax, int newMin, int newMax, int value);
float floatRange(float oldMin, float oldMax, float newMin, float newMax, float value);
int generateRand(int nMin, int nMax, uint32_t nState);
uint32_t Lehmer32(uint32_t state);
void gravity(circle* c, circle sun);
