#include <windows.h>
#include <fstream>
#include <CL/cl.h>
#include <immintrin.h>
#include <algorithm>
#include <omp.h>
#include <bitset>
#include <time.h>
#include <ctime>
#include <functional>
#include <sstream>
#include <map>
#include <iostream>
#include <vector>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include "Common.h"
#include "Computer.h"
#include "Sampler.h"
#include "Generators\Generator.h"
#include "Crossover.h"
#include "Informator.h"
#include "Historian.h"
#include "Generators\Max.h"
#include "Generators\Min.h"
#include "Generators\SMA.h"
#include "Editor.h"

void printIndividuals(Individuals& individuals, int n)
{
	int j = 0;
	for (std::multimap<int, Individual> ::iterator it = --individuals.end(); (it != individuals.begin()), j<n; --it, j++)
	{
		std::cout << (*it).first << " "
		<< (*it).second.expression <<" "
		<< (*it).second.c11 << " "
		<< (*it).second.c01 << " "
		<< "\n";
	}
}


int main(void)
{
	setlocale(LC_ALL, "Russian");
	ErrCode err = 0;

	// Считываем информацию обо всех устройствах
	Informator informator;
	Platforms platforms;
	err = informator.get_Platforms(platforms);
	if (err != 0) return 1;

	// Выбираем наиболее производительное устройство
	Device device;
	err = informator.get_Device_with_max_nCU(device, platforms);
	if (err != 0) return 1;

	// Пользователь пробивает дополнительнную информацию по выбранному устройству
	device.LDS_ByteSize = 65536;
	device.reg_ByteSize = 4;
	device.nReg_in_CU = 65536;
	device.nMaxWarp_in_CU = 40;
	device.nMaxThread_in_Warp = 64;

	// Считываем исторические данные
	Historian Historian;
	Historian.set_filePath("F:\\AdvisorWork\\History.txt");
	Historian.set_nPoint(128);
	History history;
	err = Historian.get_history(history);
	if (err != 0) return 1;

	// Рассчет образцовой и инвертированной образцовой особей
	Sampler sampler;
	sampler.set_StopLossPersent(1.0);
	sampler.set_CommissionPersent(0.05);
	sampler.set_TakeProfitPersent(1.2);
	sampler.set_TakeProfitPeriod(10);
	SampleIndividual sampleIndividual;
	err = sampler.get_SampleIndividual(sampleIndividual, history);
	if (err != 0) return 1;

	// Настройки для генерации начальной популяции
	float kRating = 1.5;
	int nMax_Individual = 5000;

	// Вывод информации об образцовой особи
	std::cout << "Образцовая особь ";
	std::cout << "c1 = " << sampleIndividual.c1 << " ";
	std::cout << "c0 = " << sampleIndividual.c0 << " ";
	std::cout << "rating = " << sampleIndividual.c1 * kRating<< "\n \n";

	// Пустая начальная популяция
	Individuals individuals;
	
	// Засекаем время
	double timeBegin = omp_get_wtime();

	// Генератор начальной популяции "Min"  
	Min min_;
	min_.set_kRating(kRating);
	min_.set_nMax_Individual(nMax_Individual);
	min_.set_Tmin(0);
	min_.set_Tmax(50);
	min_.set_Tstep(1);
	min_.set_SampleIndividual(sampleIndividual);
	min_.add_Individuals(individuals, history);

	// Генератор начальной популяции "Max"
	Max max_;
	max_.set_kRating(kRating);
	max_.set_nMax_Individual(nMax_Individual);
	max_.set_Tmin(0);
	max_.set_Tmax(50);
	max_.set_Tstep(1);
	max_.set_SampleIndividual(sampleIndividual);
	max_.add_Individuals(individuals, history);

	// Генератор начальной популяции "SMA"
	SMA sma;
	sma.set_kRating(kRating);
	sma.set_nMax_Individual(nMax_Individual);
	sma.set_Tmin(0);
	sma.set_Tmax(50);
	sma.set_Tstep(1);
	sma.set_SampleIndividual(sampleIndividual);
	sma.add_Individuals(individuals, history);

	double timeEnd = omp_get_wtime();
	double dTime = timeEnd - timeBegin;

	// Вывод лучших особей
	std::cout <<"Лучшие особи начальной популяции ";
	std::cout << "(время генерации " << dTime << " сек.)" <<"\n";

	printIndividuals(individuals, 10);
	std::cout << "\n";

	// Пустая популяция результатов спаривания
	Individuals crossedIndividuals;

	// Спариватель
	Crossover crossover;
	crossover.set_kRating(kRating);
	crossover.set_Expression("(x0 | x1 | x2) & (x3 | x4 | x5 | x6)");
	crossover.set_Kernel_filePath("F:\\Трейдинг\\VStudio\\Projects\\Ядра\\crossing.cl");
	crossover.set_nGlobal_Cicles(500);
	crossover.set_nLocal_Cicles(100*1000);
	crossover.set_SampleIndividual(sampleIndividual);
	crossover.set_nMax_Individual_Out(10000);
	crossover.set_nWarp_in_Group(4);
	crossover.set_nGroup_in_CU(5);

	// Засекаем время
	timeBegin = omp_get_wtime();

	// Работа спаривателя
	crossover.get_crossedIndividuals(crossedIndividuals, individuals, device);

	timeEnd = omp_get_wtime();
	dTime = timeEnd - timeBegin;

	std::cout  << " (время генерации " << dTime << " / " << crossover.get_computeTime() << " сек.)" << "\n";

	// Вывод лучших особей
	std::cout << "Лучшие особи итоговой популяции \n";
	printIndividuals(crossedIndividuals, 10);
	std::cout << "\n";

	//
	std::cout << "Для выхода нажмите любую клавишу";
	std::cin.get();

	return 0;
}



