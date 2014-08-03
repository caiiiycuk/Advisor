#include <CL/cl.h>
//#include <immintrin.h>
#include <algorithm>
#include <omp.h>
#include <time.h>
#include <ctime>
#include <vector>
#include <functional>
#include <bitset>
#include <map>
#include "Common.h"
#include "Historian.h"


Historian::Historian() :filePath(""), nPoint(-1), file(NULL)
{
}


void Historian::set_filePath(std::string filePath)
{
	this->filePath = filePath;
}

void Historian::set_nPoint(int nPoint)
{
	this->nPoint = nPoint;
}

ErrCode Historian::read_Header(History& history)
{
	ErrCode err = 0;

	fseek(file, 0, SEEK_SET);
	char name[10];
	//err = fscanf(file, "<DATE> <TIME> <OPEN> <HIGH> <LOW> <CLOSE> <VOL> %s\n", &name) - 1;
	err = fscanf_s(file, "<DATE> <TIME> <OPEN> <HIGH> <LOW> <CLOSE> <VOL> %s\n", &name, _countof(name)) - 1;
	if (err == 0) history.name = name;
	return err;
}

ErrCode Historian::read_Data(History& history)
{
	int verifier = 0;
	history.tms.resize(nPoint);
	history.pricesOpen.resize(nPoint);
	history.pricesHigh.resize(nPoint);
	history.pricesLow.resize(nPoint);
	history.pricesClose.resize(nPoint);
	history.values.resize(nPoint);
	history.nPoint = nPoint;

	for (int i = 0; i < nPoint; ++i) {
		//verifier += fscanf(file, "%4u%2u%2u %2u%2u%2u %f %f %f %f %f\n",
		verifier += fscanf_s(file, "%4u%2u%2u %2u%2u%2u %f %f %f %f %f\n",
			&(history.tms[i].tm_year),
			&(history.tms[i].tm_mon),
			&(history.tms[i].tm_yday),
			&(history.tms[i].tm_hour),
			&(history.tms[i].tm_min),
			&(history.tms[i].tm_sec),
			&(history.pricesOpen[i]),
			&(history.pricesHigh[i]),
			&(history.pricesLow[i]),
			&(history.pricesClose[i]),
			&(history.values[i]));
	}
	return verifier - nPoint * 11;
}


ErrCode Historian::get_history(History& history)
{
	ErrCode err = 0;
	
	//	file = fopen(filePath.c_str(), "r");
	err = fopen_s(&file, filePath.c_str(), "r");

	if (err != 0) return err;

	if (read_Header(history) != 0)
	{
		fclose(file);
		return 1;
	}

	if (read_Data(history) != 0)
	{
		fclose(file);
		return 2;
	}

	fclose(file);
	return 0;
}


Historian::~Historian()
{
}
