#include <CL/cl.h>
//#include <immintrin.h>
#include <algorithm>
#include <omp.h>
#include <time.h>
#include <ctime>
#include <stdio.h>
#include <functional>
#include <bitset>
#include <map>
#include <sstream>
#include <vector>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include "Common.h"


std::string intToString(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

std::string floatToString(float f)
{
	std::stringstream ss;
	ss << f;
	return ss.str();
}

std::string charToString(char ch)
{
	std::stringstream ss;
	ss << ch;
	return ss.str();
}

int combinations(std::vector<int>& indexA, std::vector<int>& indexB, int n)
{
	int nCombinations = n*(n - 1) / 2;
	indexA.resize(nCombinations);
	indexB.resize(nCombinations);

	int iCombination = 0;
	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			indexA[iCombination] = i;
			indexB[iCombination] = j;
			++iCombination;
		}
	}
	return nCombinations;
}

void add_Individual_to_Individuals(Individuals& individuals, Individual& individual, int nMax_Individual)
{
	if (individuals.size() < nMax_Individual)
	{
		individuals.emplace(individual.rating, individual);
	}
	else
	{
		Individuals::iterator it = individuals.begin();
		if (individual.rating > it->first)
		{
			individuals.erase(it);
			individuals.emplace(individual.rating, individual);
		}
	}
}

int find_and_replace(std::string& srcStr, const std::string& findStr, const std::string& replaceStr)
{
	int pos;
	int size = findStr.length();
	int nFind = 0;
	do
	{
		pos = srcStr.find(findStr);
		if (pos >= 0)
		{
			srcStr.replace(pos, size, replaceStr);
			++nFind;
		}
	} while (pos >= 0);

	return nFind;
}

void dynamicBitset_to_Chromoset(boost::dynamic_bitset<>& bitset, ChromoSet& chromoset)
{
	int nBit = bitset.size();
	int nChromoSector = nBit / N_BIT_IN_CHROMOSECTOR;
	chromoset.resize(nChromoSector);

	boost::to_block_range(bitset, chromoset.begin());
}
