#include <CL/cl.h>
//#include <immintrin.h>
#include <algorithm>
#include <omp.h>
#include <time.h>
#include <ctime>
#include <vector>
#include <functional>
#include <map>
#include <bitset>
#include "Common.h"
#include "Generators\Generator.h"
#include "Generators\SMA.h"


SMA::SMA()
{
	name = "SMA";
}


ErrCode SMA::calc_IndicatorPoints(std::vector<float>& indicatorPoints, const std::vector<float>& historyPoints, int t)
{
	int nPoint = historyPoints.size();
	float numerator = 0;

	// i < t
	for (int i = 0; i < t; ++i)
	{
		numerator += historyPoints[i];
		indicatorPoints[i] = -1;
	}

	// i == t
	indicatorPoints[t] = (numerator + historyPoints[t]) / (t + 1);

	// i > t
	for (int i = t + 1; i < nPoint; ++i)
	{
		indicatorPoints[i] = indicatorPoints[i-1] + (historyPoints[i] - historyPoints[i - (t + 1)]) / (t + 1);
	}
	return 0;
}


SMA::~SMA()
{
}


