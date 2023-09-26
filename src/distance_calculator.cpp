#include "distance_calculator.h"

using namespace std;

DistanceCalculator::DistanceCalculator(STL_file * stlFile, float diameter)
{
    mySTL_file = stlFile;
    laserDiameter = diameter;
}

DistanceCalculator::~DistanceCalculator()
{

}

uint8_t DistanceCalculator::findNearestIntersection(float * origin)
{
    myOrig = origin;
    mindist = 1000.0f;
    validIntersections = 0;
	uint8_t below = 0;
	uint8_t above = 0;
    for (uint32_t i = 0; i < mySTL_file->numberOfFacets; i++) {
		if(mySTL_file->useFacet[i]) {
        float vert0[3] = {
            mySTL_file->surface[i].vertex1.x,
            mySTL_file->surface[i].vertex1.y,
            mySTL_file->surface[i].vertex1.z
        };
        float vert1[3] = {
            mySTL_file->surface[i].vertex2.x,
            mySTL_file->surface[i].vertex2.y,
            mySTL_file->surface[i].vertex2.z
        };
        float vert2[3] = {
            mySTL_file->surface[i].vertex3.x,
            mySTL_file->surface[i].vertex3.y,
            mySTL_file->surface[i].vertex3.z
        };
        uint8_t retVal = moellerTrumbore(myOrig, vert0, vert1, vert2);
        if (retVal != 0) {
			if(intersect[2] < 0.0f) {
				below++;
			}
			else if(intersect[2] > maxZ) {
				above++;
			}
			else if (mindist > dist) {
                mindist = dist;
				validIntersections++;
            }
        }
    }
    dist = mindist;
    if(validIntersections == 0 && above == 0 && below == 0) {
        dist = maxZ;
		pointsOutside++;
    }
	else if(validIntersections == 0 && above > 0) {
		cerr << "[Error]: STL model within positive z-range. Possibility of laser crash. Exiting." << endl;
		exit(1);
	}
	else if(validIntersections == 0 && above == 0 && below > 0) {
		dist = maxZ; // clip distance
		pointsTooLow++;
	}
	}
    return validIntersections;
}

float DistanceCalculator::calculateLaserDistance()
{
    return -dist;
}

/*
	adapted from Tomas Möller & Ben Trumbore (1997) Fast, Minimum Storage Ray-Triangle Intersection, Journal of Graphics Tools, 2:1, 21-28, DOI: 10.1080/10867651.1997.10487468
*/
uint8_t DistanceCalculator::moellerTrumbore(float orig[3], float vert0[3], float vert1[3], float vert2[3])
{
    float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
    float det, inv_det;

    SUB(edge1, vert1, vert0);
    SUB(edge2, vert2, vert0);

    CROSS(pvec, dir, edge2);

    det = DOT(edge1, pvec);

    if (det > -EPSILON && det < EPSILON)
        return 0;
    inv_det = 1.0 / det;

    SUB(tvec, orig, vert0);

    u = DOT(tvec, pvec) * inv_det;
    if ( u < 0.0 || u > 1.0)
        return 0;

    CROSS(qvec, tvec, edge1);

    v = DOT(dir, qvec) * inv_det;
    if ( v < 0.0 || u + v > 1.0)
        return 0;

    t = DOT(edge2, qvec) * inv_det;

    ADD(intersect, orig, t, dir);

    dist = DISTANCE(intersect, orig);

    return 1;
}