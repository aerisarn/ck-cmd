#include <iostream>
#include <cmath> 

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

double rad2deg(double rad)
{
  return rad*180.0/M_PI;
}

double deg2rad(double deg)
{
	return deg * M_PI / 180.0;
}