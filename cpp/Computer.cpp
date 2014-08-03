#include <CL/cl.h>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include <iostream>
#include "Common.h"
#include "Editor.h"
#include "Computer.h"


Computer::Computer() : program_(NULL), kernel_(NULL), queue_(NULL), context_(NULL), kernel_Source(""), kernel_Name(""), nThread(0), nThread_in_Group(0), nArg(0), 
					   buildTime(0), computeTime(0)
{
}


void Computer::set_Device(Device device)
{
	this->device = device;
}

void Computer::set_Kernel_Source(std::string kernel_Source)
{
	this->kernel_Source = kernel_Source;
}

void Computer::set_Kernel_Name(std::string kernel_Name)
{
	this->kernel_Name = kernel_Name;
}

void Computer::set_nThread(size_t nThread)
{
	this->nThread = nThread;
}

void Computer::set_nThread_in_Group(size_t nThread_in_Group)
{
	this->nThread_in_Group = nThread_in_Group;
}


ErrCode Computer::build_kernel()
{
	ErrCode err = 0;

	const char* source_char =  kernel_Source.c_str();

	context_ = clCreateContext(0, 1, &device.id, NULL, NULL, &err);
	if (err != 0) return err;

	queue_ = clCreateCommandQueue(context_, device.id, 0, &err);
	if (err != 0) return err;

	program_ = clCreateProgramWithSource(context_, 1, (const char **)& source_char, NULL, &err);
	if (err != 0) return err;

	double timeBegin = omp_get_wtime();

	err = clBuildProgram(program_, 0, NULL, NULL, NULL, NULL);
	if (err != 0) return err;

	double timeEnd = omp_get_wtime();
	buildTime = timeEnd - timeBegin;

	kernel_ = clCreateKernel(program_, kernel_Name.c_str(), &err);
	if (err != 0) return err;

	return 0;
}


ErrCode Computer::add_Local(size_t byteSize)
{
	ErrCode err = 0;
	err = clSetKernelArg(kernel_, nArg, byteSize, (void*)NULL);
	if (err == 0)
	{
		++nArg;
	}

	return err;
}



ErrCode Computer::compute()
{
	ErrCode err = 0;
	double timeBegin = omp_get_wtime();

	// Запускаем ядро
	err = clEnqueueNDRangeKernel(queue_, kernel_, 1, NULL, &nThread, &nThread_in_Group, 0, NULL, NULL);
	if (err != 0) return err;

	// Ожидаем завершения выполнения очереди команд.
	err = clFinish(queue_);

	double timeEnd = omp_get_wtime();
	computeTime = timeEnd - timeBegin;

	// Считываем данные
	for (int i = 0; i < buffers.size(); ++i)
	{
		if (buffers[i].readFlag)
		{
			err = clEnqueueReadBuffer(queue_, buffers[i].data, CL_TRUE, 0, buffers[i].byteSize, buffers[i].pointer, 0, NULL, NULL);
		}
	}

	return err;
}


double Computer::get_buildTime()
{
	return buildTime;
}

double Computer::get_computeTime()
{
	return computeTime;
}


ErrCode Computer::clear()
{
	ErrCode err = 0;
	//
	kernel_Source = "";
	kernel_Name = "";
	nThread = 0;
	nThread_in_Group = 0;
	nArg = 0;
	buildTime = 0;
	computeTime = 0;
	
	//clFlush
	//
	int n = buffers.size();
	for (int i = 0; i < n; ++i)
	{
		if (buffers[i].data) err = clReleaseMemObject(buffers[i].data);
	}
	buffers.clear();

	//
	if (program_) err = clReleaseProgram(program_);
	if (kernel_) err = clReleaseKernel(kernel_);
	if (queue_) err = clReleaseCommandQueue(queue_);
	if (context_) err = clReleaseContext(context_);
	//
	program_ = NULL;
	kernel_ = NULL;
	queue_ = NULL;
	context_ = NULL;
	//
	return 0;
}



Computer::~Computer()
{
	clear();
}
