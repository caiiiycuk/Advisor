#include <CL/cl.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <ctime>
#include "boost/dynamic_bitset.hpp"
#include <functional>
#include <vector>
#include <map>
#include "Common.h"
#include "Computer.h"
#include "Editor.h"
#include "Crossover.h"


Crossover::Crossover() : kRating(-1), nIndividual_in_Loc(-1), nGlobal_Cicles(-1), nMax_Individual_Out(-1), kernel_filePath(""), expression(""), nGroup_in_CU(0), nWarp_in_Group(0)
{

}	

void Crossover::set_kRating(float kRating)
{
	this->kRating = kRating;
}

void Crossover::set_Kernel_filePath(std::string kernel_filePath)
{
	this->kernel_filePath = kernel_filePath;
}

void Crossover::set_Expression(std::string expression)
{
	this->expression = expression;
}


void Crossover::set_nGlobal_Cicles(int nGlobal_Cicles)
{
	this->nGlobal_Cicles = nGlobal_Cicles;
}

void Crossover::set_nLocal_Cicles(int nLocal_Cicles)
{
	this->nLocal_Cicles = nLocal_Cicles;
}

void Crossover::set_SampleIndividual(const SampleIndividual& sampleIndividual)
{
	this->sampleIndividual = sampleIndividual;
}

void Crossover::set_nMax_Individual_Out(int nMax_Individual_Out)
{
	this->nMax_Individual_Out = nMax_Individual_Out;
}

void Crossover::calc_nParent()
{
	std::string str = expression;
	int i = 0;
	int nFind = 0;
	do
	{
		std::string findStr = "x" + intToString(i);
		nFind = find_and_replace(str, findStr, "");
		if (nFind>0) ++i;
	} while (nFind > 0);

	nParent = i;
}

void Crossover::set_nGroup_in_CU(int nGroup_in_CU)
{
	this->nGroup_in_CU = nGroup_in_CU;
}

void Crossover::set_nWarp_in_Group(int nWarp_in_Group)
{
	this->nWarp_in_Group = nWarp_in_Group;
}

ErrCode Crossover::get_crossedIndividuals(Individuals& crossedIndividuals, Individuals& individuals, const Device& device)
{
	ErrCode err = 0;

	// Параметры
	int nThread_in_Warp = device.nMaxThread_in_Warp;
	int nThread_in_Group = nThread_in_Warp * nWarp_in_Group;
	int nThread = nThread_in_Group * nGroup_in_CU * device.max_compute_units;

	// Локальные
	int loc_sample_ByteSize = sampleIndividual.nByte_in_Chromoset;
	int loc_sampleInvert_ByteSize = sampleIndividual.nByte_in_Chromoset;
	int nIndividual_in_Loc = (device.LDS_ByteSize / nGroup_in_CU - loc_sample_ByteSize - loc_sampleInvert_ByteSize) / (sizeof(int)+sampleIndividual.nByte_in_Chromoset);
	int loc_indexes_ByteSize = nIndividual_in_Loc * sizeof(int);
	int loc_Individuals_ByteSize = nIndividual_in_Loc * sampleIndividual.nByte_in_Chromoset;
	int loc_ByteSize = loc_sample_ByteSize + loc_sampleInvert_ByteSize + loc_indexes_ByteSize + loc_Individuals_ByteSize;

	Editor editor;

	err = editor.import_(kernel_filePath);
	if (err != 0) return err;
	editor.replace("$$EXPRESSION", expression);
	calc_nParent();
	editor.replace("$$N_PARENT", intToString(nParent));
	editor.replace("$$N_CHROMOSECTOR_IN_CHROMOSET", intToString(sampleIndividual.chromoSectors.size()));
	editor.replace("$$N_CHROMOSET_IN_LOC", intToString(nIndividual_in_Loc));
	editor.replace("$$N_CHROMOSET_IN_GLOB", intToString(individuals.size()-10));
	int min_rating = (*(--individuals.end())).second.rating;
	editor.replace("$$START_RATING", intToString(min_rating));
	editor.replace("$$N_GLOBAL_CICLES", intToString(nGlobal_Cicles));
	editor.replace("$$N_LOCAL_CICLES", intToString(nLocal_Cicles));
	editor.replace("$$K_RATING", floatToString(kRating));
	editor.replace("$$KOEF_A", intToString(15678));
	editor.replace("$$KOEF_C", intToString(34302));

	editor.expand("$$EXTRACT_N_PARENT", nParent);
			
	editor.export_(kernel_filePath + KERNEL_FILE_SUFFIX);

	std::string kernel_Source;
	editor.get_string(kernel_Source);
	editor.clear();

	Computer computer;

	computer.set_Device(device);
	computer.set_Kernel_Name("crossing");
	computer.set_Kernel_Source(kernel_Source);
	computer.set_nThread_in_Group(nThread_in_Group);
	computer.set_nThread(nThread);

	// Строим (компиляция ядра, создание контекста...) 
	err = computer.build_kernel();
	if (err != 0) return err;

	DataIn dataIn;
	err = get_DataIn_from_Individuals(dataIn, individuals);

	// Входа
	computer.add_In(dataIn.chromoSectors);
	computer.add_In(sampleIndividual.chromoSectors);
	computer.add_In(sampleIndividual.invertChromoSectors);

	computer.add_Local(loc_ByteSize);

	DataOut dataOut(nThread, nParent);

	// Выхода
	computer.add_Out(dataOut.glob_indexes);
	computer.add_Out(dataOut.ratings);
	computer.add_Out(dataOut.c11s);
	computer.add_Out(dataOut.c01s);

	// Запускаем
	computer.compute();

	// Восстанавливаем особей
	err = get_Individuals_from_DataOut(crossedIndividuals, dataOut);

	computeTime = computer.get_computeTime();

	// Освобождение ресурсов
	computer.clear();

	return 0;
}

// Подготовка входных данных (формируем вектор хромосом и имен из мультимэпа особей)
ErrCode Crossover::get_DataIn_from_Individuals(DataIn& dataIn, Individuals& individuals)
{
	Individuals::iterator itIndividual = individuals.begin();

	size_t nIndividual = individuals.size();
	int nChromoSector_in_Chromoset = sampleIndividual.nChromoSector_in_Chromoset;
	dataIn.chromoSectors.resize(nIndividual*nChromoSector_in_Chromoset);
	dataIn.expressions.resize(nIndividual);

	for (int i = 0; i < nIndividual; ++i)
	{
		memcpy(&(dataIn.chromoSectors[i*nChromoSector_in_Chromoset]), &(itIndividual->second.chromoSectors[0]), sampleIndividual.nByte_in_Chromoset);
		dataIn.expressions[i] = itIndividual->second.expression;
		++itIndividual;
	}
	return 0;
}

/*
void Crossover::get_expression(std::string& expression, Strings& individualExpressions)
{
	int nIndividualExpressions = individualExpressions.size();
	for (int i = 0; i < nIndividualExpressions; ++i)
	{
		std::string findStr = "x" + intToString(i);
		std::string replaceStr = individualExpressions[i];
		find_and_replace(expression, findStr, replaceStr);
	}
}


void Crossover::get_individualExpressions(Strings& individualExpressions, int iParent)
{
	for (int i = 0; i < nParent; ++i)
	{
		int iIndividual = dataOut.glob_indexes[iParent++];
		individualExpressions[i] = dataIn.expressions[iIndividual];
	}
}
*/

// Восстановление поколения по выходному вектору
ErrCode Crossover::get_Individuals_from_DataOut(Individuals& individuals, DataOut& dataOut)
{
	Individuals::iterator itIndividual;
	for (int i = 0; i < dataOut.nCrossedIndividual; ++i)
	{
		/*
		Strings individualExpressions;
		get_individualExpressions(individualExpressions, i * nParent);
		
		std::string expression;
		get_expression(expression, individualExpressions);
		*/
		Individual individual("", dataOut.c11s[i], dataOut.c01s[i], dataOut.ratings[i]);
		add_Individual_to_Individuals(individuals, individual, nMax_Individual_Out);
	}
	return 0;
}

double Crossover::get_computeTime()
{
	return computeTime;
}

Crossover::~Crossover()
{

}


