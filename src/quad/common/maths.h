#ifndef __MATHS_H
#define __MATHS_H

#include <math.h>

// Use floating point M_PI instead explicitly.
#define M_PIf       3.14159265358979323846f

#define RAD    (M_PIf / 180.0f)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(x) ((x) > 0 ? (x) : -(x))

typedef struct stdev_s {
	float m_oldM, m_newM, m_oldS, m_newS;
	int m_n;
}stdev_t;

void devClear(stdev_t *dev);
void devPush(stdev_t *dev, float x);
float devVariance(stdev_t *dev);
float devStandardDeviation(stdev_t *dev);

#endif	// __MATHS_H
