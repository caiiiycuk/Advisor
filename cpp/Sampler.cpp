#include <CL/cl.h>
#include <iostream>
//#include <immintrin.h>
#include <algorithm>
#include <omp.h>
#include <time.h>
#include <ctime>
#include <vector>
#include <functional>
#include <bitset>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <map>
#include "Common.h"
#include "Sampler.h"

Sampler::Sampler() :stopLossPersent(0), commissionPersent(0), takeProfitPersent(0), takeProfitPeriod(0)
{
}


void Sampler::set_StopLossPersent(float stopLossPersent)
{
	this->stopLossPersent = stopLossPersent;
}

void Sampler::set_CommissionPersent(float commissionPersent)
{
	this->commissionPersent = commissionPersent;
}

void Sampler::set_TakeProfitPersent(float takeProfitPersent)
{
	this->takeProfitPersent = takeProfitPersent;
}

void Sampler::set_TakeProfitPeriod(int takeProfitPeriod)
{
	this->takeProfitPeriod = takeProfitPeriod;
}


ErrCode Sampler::get_SampleIndividual(SampleIndividual& sampleIndividual, const History& history)
{
	int nBit_in_Chromoset = history.nPoint;
	int nByte_in_Chromoset = nBit_in_Chromoset / 8;
	int nChromoSector_in_Chromoset = nByte_in_Chromoset / sizeof(ChromoSector);

	boost::dynamic_bitset<> sampleChromosomes(nBit_in_Chromoset);

	// ���� �������� �������. 
	for (int i = 0; i < nBit_in_Chromoset; ++i)
	{
		// ������ ���������� �������.
		float priceBuy = history.pricesHigh[i];
		float comission = priceBuy*commissionPersent / 100;

		// ������� ������������ ���� ��������.
		float deltaStopLoss = priceBuy*stopLossPersent / 100;
		float deltaTakeProfit = priceBuy*takeProfitPersent / 100;
		float priceStopLoss = priceBuy - deltaStopLoss;
		float priceTakeProfit = priceBuy + deltaTakeProfit;

		// ������ ������� ������ ���������� (�� TakeProfit) �������. 
		int j_max = i + takeProfitPeriod;
		if (j_max > nBit_in_Chromoset) j_max = nBit_in_Chromoset;

		// ���� ������ ���������� (�� TakeProfit) �������.
		bool check = false;
		for (int j = i + 1; j < j_max; ++j)
		{
			if (history.pricesLow[j] > priceStopLoss)
			{
				if (history.pricesHigh[j] >= priceTakeProfit)
				{
					check = true;
					break;
				};
			}
			else
				break;
		}
		// ���������� ����������� ������.
		sampleChromosomes[i] = check;
	}

	// ���������� �����	
	dynamicBitset_to_Chromoset(sampleChromosomes, sampleIndividual.chromoSectors);
	sampleIndividual.c1 = sampleChromosomes.count();
	sampleIndividual.c0 = nBit_in_Chromoset - sampleIndividual.c1;

	// ��������������� ���������� �����	
	boost::dynamic_bitset<> invertSampleChromosomes = ~sampleChromosomes;
	dynamicBitset_to_Chromoset(invertSampleChromosomes, sampleIndividual.invertChromoSectors);

	sampleIndividual.nBit_in_Chromoset = nBit_in_Chromoset;
	sampleIndividual.nByte_in_Chromoset = nByte_in_Chromoset;
	sampleIndividual.nChromoSector_in_Chromoset = nChromoSector_in_Chromoset;

	return 0;
}


Sampler::~Sampler()
{
}

