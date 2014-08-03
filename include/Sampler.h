/****
* Назначение:
* компьютер расчета качества
****/

#pragma once

void dynamicBitset_to_Chromoset(boost::dynamic_bitset<>& bitset, ChromoSet& chromoset);

class Sampler
{
public:
	Sampler();
	void set_StopLossPersent(float stopLossPersent);
	void set_CommissionPersent(float commissionPersent);
	void set_TakeProfitPersent(float takeProfitPersent);
	void set_TakeProfitPeriod(int takeProfitPeriod);
	ErrCode get_SampleIndividual(SampleIndividual& sampleIndividual, const History& history);
	~Sampler();
private:
	float stopLossPersent;
	float commissionPersent;
	float takeProfitPersent;
	int takeProfitPeriod;
};
