#ifndef distcalc_h
#define distcalc_h

#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <algorithm>

#include "STL_file.h"

#define EPSILON 0.000001
#define CROSS(dest, v1, v2)\
	dest[0] = v1[1] * v2[2] - v1[2] * v2[1];\
	dest[1] = v1[2] * v2[0] - v1[0] * v2[2];\
	dest[2] = v1[0] * v2[1] - v1[1] * v2[0];
#define DOT(v1, v2)(v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2])
#define SUB(dest, v1, v2)\
	dest[0] = v1[0] - v2[0];\
	dest[1] = v1[1] - v2[1];\
	dest[2] = v1[2] - v2[2];
#define ADD(dest, v1, t, dir)\
	dest[0] = v1[0] + t * dir[0];\
	dest[1] = v1[1] + t * dir[1];\
	dest[2] = v1[2] + t * dir[2];
#define DISTANCE(v1, v2)({\
	float diffX = v1[0] - v2[0];\
	float diffY = v1[1] - v2[1];\
	float diffZ = v1[2] - v2[2];\
	sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ);\
})

class DistanceCalculator
{
	public:

		DistanceCalculator(STL_file * stlFile, float diameter);

		~DistanceCalculator();

		uint8_t findNearestIntersection(float * origin);

		float calculateLaserDistance();

		size_t pointsOutside = 0;
		size_t pointsTooLow = 0;

		float laserDiameter = 0.0f;

		float cal1 = -4.5584f;
		float cal2 = 34.71f;

		float maxZ = cal1 * laserDiameter + cal2;

	private:

		STL_file * mySTL_file;

		std::ifstream config;

		float * myOrig;

		const float dir[3] = {
			0.0f,
			0.0f,
			-1.0f
		};

		float t;
		float u;
		float v;
		float intersect[3];

		float dist;
		float mindist = 1000.0f;
		uint8_t validIntersections = 0;

		float laserDist = 0.0f;

		uint8_t moellerTrumbore(float orig[3], float vert0[3], float vert1[3], float vert2[3]);

};

#endif