/****
* Назначение:
* компьютер расчета качества
****/

#pragma once

std::string intToString(int i);
std::string floatToString(float f);
void add_Individual_to_Individuals(Individuals& individuals, Individual& idividual, int nMax_Individual);
int find_and_replace(std::string& srcStr, const std::string& findStr, const std::string& replaceStr);

class Crossover 
{

public:
	Crossover();
	struct DataIn
	{
		DataIn() {};
		ChromoSectors chromoSectors;
		Strings expressions;
		~DataIn() {};
	};

	struct DataOut
	{
		DataOut(int nIndividual, int nParent) : nCrossedIndividual(nIndividual), glob_indexes(nIndividual*nParent), ratings(nIndividual), c11s(nIndividual), c01s(nIndividual) {};
		std::vector<int> glob_indexes;
		std::vector<int> ratings;
		std::vector<int> c11s;
		std::vector<int> c01s;
		int nCrossedIndividual;
		~DataOut() {};
	};

	void set_kRating(float kRating);
	void set_Kernel_filePath(std::string kernel_filePath);
	void set_Expression(std::string expression);
	void set_nGlobal_Cicles(int nGlobal_Cicles);
	void set_nLocal_Cicles(int nLocal_Cicles);
	void set_nMax_Individual_Out(int nMax_Individual_Out);
	void set_SampleIndividual(const SampleIndividual& sampleIndividual);
	void set_nGroup_in_CU(int nGroup_in_CU);
	void set_nWarp_in_Group(int nWarp_in_Group);
	ErrCode get_crossedIndividuals(Individuals& crossedIndividuals, Individuals& individuals, const Device& device);
	double get_computeTime();
	~Crossover();
private:
	float kRating;
	int nParent;
	int nIndividual_in_Loc;
	int nGlobal_Cicles;
	int nLocal_Cicles;
	int nMax_Individual_Out;
	int nGroup_in_CU;
	int nWarp_in_Group;
	double computeTime;
	std::string kernel_filePath;
	std::string expression;
	SampleIndividual sampleIndividual;
	void calc_nParent();
	void get_expression(std::string& expression, Strings& individualExpressions);
	void get_individualExpressions(Strings& individualExpressions, int index);
	ErrCode get_DataIn_from_Individuals(DataIn& dataIn, Individuals& individuals);
	ErrCode get_Individuals_from_DataOut(Individuals& individuals, DataOut& dataOut);
};
