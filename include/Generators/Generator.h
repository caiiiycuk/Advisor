#pragma once

std::string intToString(int i);
int combinations(std::vector<int>& indexA, std::vector<int>& indexB, int n);
void add_Individual_to_Individuals(Individuals& individuals, Individual& idividual, int nMax_Individual);

class Generator
{
public:
	Generator();

	struct ChromoSectorList
	{
		ChromoSectorList() : greater(0), less(0), greater_or_equal(0), less_or_equal(0), equal(0) {};
		ChromoSector greater;
		ChromoSector less;
		ChromoSector greater_or_equal;
		ChromoSector less_or_equal;
		ChromoSector equal;
		~ChromoSectorList() {};
	};

	struct ScoreList
	{
		ScoreList()			: greater_c11(0), greater_c01(0), less_c11(0), less_c01(0), greater_or_equal_c11(0), greater_or_equal_c01(0), less_or_equal_c11(0), less_or_equal_c01(0),
							  equal_c11(0), equal_c01(0) {};
		int greater_c11;
		int greater_c01;
		int less_c11;
		int less_c01;
		int greater_or_equal_c11;
		int greater_or_equal_c01;
		int less_or_equal_c11;
		int less_or_equal_c01;
		int equal_c11;
		int equal_c01;
		~ScoreList() {};
	};

	struct IndividualList
	{
		IndividualList()					: greater(">"), less("<"), greater_or_equal(">="), less_or_equal("<="), equal("=") {};
		
		IndividualList(int nChromoSector)	: greater(">", nChromoSector), less("<", nChromoSector), greater_or_equal(">=", nChromoSector), less_or_equal("<=", nChromoSector), 
											  equal("==", nChromoSector) {};
		Individual greater;
		Individual less;
		Individual greater_or_equal;
		Individual less_or_equal;
		Individual equal;
		~IndividualList() {};
	};

	struct Indicator
	{
		Indicator()							: name("") {};						
		Indicator(int nPoint)				: name(""),	points(nPoint) {};
		std::vector<float> points;
		std::string name;
		~Indicator() {};
	};

	typedef std::vector<Indicator> Indicators;

	void set_kRating(float kRating);
	void set_Tmin(int tMin);
	void set_Tmax(int tMax);
	void set_Tstep(int tStep);
	void set_nMax_Individual(int nMax_Individual);
	void set_SampleIndividual(const SampleIndividual& sampleIndividual);
	void add_Individuals(Individuals& individuals, const History& history);
	~Generator();
protected:
	float kRating;
	int tMin;
	int tMax;
	int tStep;
	int nMax_Individual;
	int nStep;
	Indicators indicators;
	SampleIndividual sampleIndividual;
	std::string name;
	//
	void add_SomeIndicators(const std::vector<float>& historyPoints, std::string addName);
	void add_SomeIndividuals(Individuals& individuals);
	virtual ErrCode calc_IndicatorPoints(std::vector<float>& indicatorPoints, const std::vector<float>& historyPoints, int t) = 0;
	void calc_ChromoSectorList(ChromoSectorList& chromoSectorList, const float* f1, const float* f2);
	void calc_ScoreList(ScoreList& scoreList, const ChromoSectorList& chromoSectorList, ChromoSector chromoSector, ChromoSector invertChromosector);
	void calc_IndividualList(IndividualList& individualList, const Indicator& indicatorA, const Indicator& indicatorB);
	Individual* get_BestIndividual(IndividualList& individualList);
};

