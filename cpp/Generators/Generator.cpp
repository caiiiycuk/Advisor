#include <CL/cl.h>
#include <immintrin.h>
//#include <popcntintrin.h>
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

Generator::Generator() : kRating(0), tMin(-1), tMax(-1), tStep(-1), nMax_Individual(-1), nStep(-1), name("")
{
}


void Generator::set_kRating(float kRating)
{
	this->kRating = kRating;
}

void Generator::set_Tmin(int tMin)
{
	this->tMin = tMin;
}

void Generator::set_Tmax(int tMax)
{
	this->tMax = tMax;
}

void Generator::set_Tstep(int tStep)
{
	this->tStep = tStep;
}

void Generator::set_nMax_Individual(int nMax_Individual)
{
	this->nMax_Individual = nMax_Individual;
}

void Generator::set_SampleIndividual(const SampleIndividual& sampleIndividual)
{
	this->sampleIndividual = sampleIndividual;
}

void Generator::add_Individuals(Individuals& individuals, const History& history)
{
	nStep = (tMax - tMin) / tStep + 1;

	// Группа 1
	indicators.clear();
	indicators.reserve(nStep*4);
	add_SomeIndicators(history.pricesClose, "Close");
	add_SomeIndicators(history.pricesOpen, "Open");
	add_SomeIndicators(history.pricesLow, "Low");
	add_SomeIndicators(history.pricesHigh, "High");
	add_SomeIndividuals(individuals);

	// Группа 2
	indicators.clear();
	indicators.reserve(nStep);
	add_SomeIndicators(history.values, "Value");
	add_SomeIndividuals(individuals);
	
	// Освобождаем ресурсы
	indicators.clear();
}


void Generator::add_SomeIndicators(const std::vector<float>& historyPoints, std::string addName)
{
	int t = tMin;
	int nPoint = historyPoints.size();

	for (int i = 0; i < nStep; ++i)
	{
		// Добавляем пустой индикатор
		indicators.push_back(Indicator(nPoint));
		// Вызываем виртуальную функцию (см. реализации в разных генераторах), заполняющую вектор "points" добавленного индикатора 
		calc_IndicatorPoints(indicators.back().points, historyPoints, t);
		// Заполняем поле имени добавленного индикатора 
		indicators.back().name = name + addName + "(" + intToString(t) + ")";
		t += tStep;
	}
}


void Generator::add_SomeIndividuals(Individuals& individuals)
{
	std::vector<int> indexA;
	std::vector<int> indexB;
	int nCombinations = combinations(indexA, indexB, indicators.size());
	
	#pragma omp parallel
	{
		#pragma omp for 
		for (int i = 0; i < nCombinations; ++i)
		{
			IndividualList individualList(sampleIndividual.nChromoSector_in_Chromoset);
			calc_IndividualList(individualList, indicators[indexA[i]], indicators[indexB[i]]);

			Individual* bestIndividual = get_BestIndividual(individualList);

			if (bestIndividual != NULL)
			{
				#pragma omp critical
				{
					add_Individual_to_Individuals(individuals, *bestIndividual, nMax_Individual);
				}
			}
		}
	}
}


void Generator::calc_IndividualList(IndividualList& individualList, const Indicator& indicatorA, const Indicator& indicatorB)
{
	int nChromoSector_in_Chromoset = sampleIndividual.nChromoSector_in_Chromoset;
	int nChromosome_in_ChromoSector = N_BIT_IN_CHROMOSECTOR;
	
	int iChromosome = 0;
	for (int i = 0; i < nChromoSector_in_Chromoset; ++i)
	{
		ChromoSectorList chromoSectorList;

		calc_ChromoSectorList(chromoSectorList, &indicatorA.points[iChromosome], &indicatorB.points[iChromosome]);

		ScoreList scoreList;
		calc_ScoreList(scoreList, chromoSectorList, sampleIndividual.chromoSectors[i], sampleIndividual.invertChromoSectors[i]);

		// Выходная структура
		//
		individualList.greater.chromoSectors[i] = chromoSectorList.greater;
		individualList.greater.c11 += scoreList.greater_c11;
		individualList.greater.c01 += scoreList.greater_c01;
		//
		individualList.less.chromoSectors[i] = chromoSectorList.less;
		individualList.less.c11 += scoreList.less_c11;
		individualList.less.c01 += scoreList.less_c01;
		//
		individualList.greater_or_equal.chromoSectors[i] = chromoSectorList.greater_or_equal;
		individualList.greater_or_equal.c11 += scoreList.greater_or_equal_c11;
		individualList.greater_or_equal.c01 += scoreList.greater_or_equal_c01;
		//
		individualList.less_or_equal.chromoSectors[i] = chromoSectorList.less_or_equal;
		individualList.less_or_equal.c11 += scoreList.less_or_equal_c11;
		individualList.less_or_equal.c01 += scoreList.less_or_equal_c01;
		//
		individualList.equal.chromoSectors[i] = chromoSectorList.equal;
		individualList.equal.c11 += scoreList.equal_c11;
		individualList.equal.c01 += scoreList.equal_c01;

		//
		iChromosome += nChromosome_in_ChromoSector;
	}

	individualList.greater.rating = individualList.greater.c11 * kRating - individualList.greater.c01;
	individualList.less.rating = individualList.less.c11 * kRating - individualList.less.c01;
	individualList.greater_or_equal.rating = individualList.greater_or_equal.c11 * kRating - individualList.greater_or_equal.c01;
	individualList.less_or_equal.rating = individualList.less_or_equal.c11 * kRating - individualList.less_or_equal.c01;
	individualList.equal.rating = individualList.equal.c11 * kRating - individualList.equal.c01;

	individualList.greater.certified = (individualList.greater.c11>0) && (individualList.greater.c11<sampleIndividual.c1);
	individualList.less.certified = (individualList.less.c11>0) && (individualList.less.c11<sampleIndividual.c1);
	individualList.greater_or_equal.certified = (individualList.greater_or_equal.c11>0) && (individualList.greater_or_equal.c11<sampleIndividual.c1);
	individualList.less_or_equal.certified = (individualList.less_or_equal.c11>0) && (individualList.less_or_equal.c11<sampleIndividual.c1);
	individualList.equal.certified = (individualList.equal.c11>0) && (individualList.equal.c11<sampleIndividual.c1);

	if (!individualList.greater.certified)			individualList.greater.rating = MIN_RATING;
	if (!individualList.less.certified)				individualList.less.rating = MIN_RATING;
	if (!individualList.greater_or_equal.certified) individualList.greater_or_equal.rating = MIN_RATING;
	if (!individualList.less_or_equal.certified)	individualList.less_or_equal.rating = MIN_RATING;
	if (!individualList.equal.certified)			individualList.equal.rating = MIN_RATING;

	individualList.greater.expression				= indicatorA.name + individualList.greater.expression + indicatorB.name;
	individualList.less.expression					= indicatorA.name + individualList.less.expression + indicatorB.name;
	individualList.greater_or_equal.expression		= indicatorA.name + individualList.greater_or_equal.expression + indicatorB.name;
	individualList.less_or_equal.expression			= indicatorA.name + individualList.less_or_equal.expression + indicatorB.name;
	individualList.equal.expression					= indicatorA.name + individualList.equal.expression + indicatorB.name;
}

void Generator::calc_ChromoSectorList(ChromoSectorList& chromoSectorList, const float* f1, const float* f2)
{
	__m256* f1_ = (__m256*)f1;
	__m256* f2_ = (__m256*)f2;

	__m256i ymmG;
	__m256i ymmL;

	// Точки 0..7
	__m256 ymm1 = f1_[0];
	__m256 ymm2 = f2_[0];
	ymmG.m256i_i8[0] = _mm256_movemask_ps(_mm256_cmp_ps(ymm1, ymm2, _CMP_GT_OQ));
	ymmL.m256i_i8[0] = _mm256_movemask_ps(_mm256_cmp_ps(ymm1, ymm2, _CMP_LT_OQ));

	// Точки 8..15
	ymm1 = f1_[1];
	ymm2 = f2_[1];
	ymmG.m256i_i8[1] = _mm256_movemask_ps(_mm256_cmp_ps(ymm1, ymm2, _CMP_GT_OQ));
	ymmL.m256i_i8[1] = _mm256_movemask_ps(_mm256_cmp_ps(ymm1, ymm2, _CMP_LT_OQ));
	// Точки 16..23
	ymm1 = f1_[2];
	ymm2 = f2_[2];
	ymmG.m256i_i8[2] = _mm256_movemask_ps(_mm256_cmp_ps(ymm1, ymm2, _CMP_GT_OQ));
	ymmL.m256i_i8[2] = _mm256_movemask_ps(_mm256_cmp_ps(ymm1, ymm2, _CMP_LT_OQ));
	// Точки 24..31
	ymm1 = f1_[3];
	ymm2 = f2_[3];
	ymmG.m256i_i8[3] = _mm256_movemask_ps(_mm256_cmp_ps(ymm1, ymm2, _CMP_GT_OQ));
	ymmL.m256i_i8[3] = _mm256_movemask_ps(_mm256_cmp_ps(ymm1, ymm2, _CMP_LT_OQ));

	// Выходная структура
	chromoSectorList.greater			= ymmG.m256i_i32[0];
	chromoSectorList.less				= ymmL.m256i_i32[0];
	chromoSectorList.greater_or_equal	= ~chromoSectorList.less;
	chromoSectorList.less_or_equal		= ~chromoSectorList.greater;
	chromoSectorList.equal				= chromoSectorList.greater_or_equal & chromoSectorList.less_or_equal;

}

void Generator::calc_ScoreList(ScoreList& scoreList, const ChromoSectorList& chromoSectorList, ChromoSector sampleChromoSector, ChromoSector invertSampleChromosector)
{

	// Выходная структура
	scoreList.greater_c11		   = _mm_popcnt_u32(sampleChromoSector & chromoSectorList.greater);
	scoreList.greater_c01		   = _mm_popcnt_u32(invertSampleChromosector & chromoSectorList.greater);

	scoreList.less_c11			   = _mm_popcnt_u32(sampleChromoSector & chromoSectorList.less);
	scoreList.less_c01			   = _mm_popcnt_u32(invertSampleChromosector & chromoSectorList.less);

	scoreList.greater_or_equal_c11 = _mm_popcnt_u32(sampleChromoSector & chromoSectorList.greater_or_equal);
	scoreList.greater_or_equal_c01 = _mm_popcnt_u32(invertSampleChromosector & chromoSectorList.greater_or_equal);

	scoreList.less_or_equal_c11    = _mm_popcnt_u32(sampleChromoSector & chromoSectorList.less_or_equal);
	scoreList.less_or_equal_c01    = _mm_popcnt_u32(invertSampleChromosector & chromoSectorList.less_or_equal);

	scoreList.equal_c11			   = _mm_popcnt_u32(sampleChromoSector & chromoSectorList.equal);
	scoreList.equal_c01			   = _mm_popcnt_u32(invertSampleChromosector & chromoSectorList.equal);
}


Individual* Generator::get_BestIndividual(IndividualList& individualList)
{
	int ratingMax = MIN_RATING;
	int iBest = -1;

	if (individualList.greater.rating > ratingMax)
	{
		ratingMax = individualList.greater.rating;
		iBest = 1;
	}

	if (individualList.less.rating > ratingMax)
	{
		ratingMax = individualList.less.rating;
		iBest = 2;
	}

	if (individualList.greater_or_equal.rating > ratingMax)
	{
		ratingMax = individualList.greater_or_equal.rating;
		iBest = 3;
	}

	if (individualList.less_or_equal.rating > ratingMax)
	{
		ratingMax = individualList.less_or_equal.rating;
		iBest = 4;
	}

	if (individualList.equal.rating > ratingMax)
	{
		ratingMax = individualList.equal.rating;
		iBest = 5;
	}

	switch (iBest)
	{
		case 1: return &individualList.greater;				break;
		case 2: return &individualList.less;				break;
		case 3: return &individualList.greater_or_equal;	break;
		case 4: return &individualList.less_or_equal;		break;
		case 5: return &individualList.equal;				break;
		default:return NULL;							    break;
	}

	return NULL;
}
	


Generator::~Generator()
{
}
