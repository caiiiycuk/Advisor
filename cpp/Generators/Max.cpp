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
#include "Generators\Max.h"


Max::Max()
{
	name = "Max";
}


ErrCode Max::calc_IndicatorPoints(std::vector<float>& indicatorPoints, const std::vector<float>& historyPoints, int t)
{
	int nPoint = historyPoints.size();

	#pragma omp parallel 
	{
		#pragma omp for 
		for (int i = 0; i< nPoint; ++i)
		{
			if (i >= t)
			{
				auto itBeg = &historyPoints[i - t];
				auto itEnd = &historyPoints[i] + 1;
				indicatorPoints[i] = *std::max_element(itBeg, itEnd);
			}
			else
			{
				indicatorPoints[i] = -1;
			}
		}
	}
	return 0;
}


Max::~Max()
{
}
