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
#include <iostream>
#include "Common.h"
#include "Generators\Generator.h"
#include "Generators\Min.h"

Min::Min()
{
	name = "Min";
}


ErrCode Min::calc_IndicatorPoints(std::vector<float>& indicatorPoints, const std::vector<float>& historyPoints, int t)
{
	int nPoint = historyPoints.size();

	#pragma omp parallel 
	{
		#pragma omp for 
		for (int i = 0; i< nPoint; ++i)
		{
			if (i >= t)
			{
				auto itBeg = &historyPoints[i-t];
				auto itEnd = &historyPoints[i]+1;
				indicatorPoints[i] = *std::min_element(itBeg, itEnd);
			}
			else
			{
				indicatorPoints[i] = -1;
			}
		}
	}
	return 0;
}


Min::~Min()
{
}
