#pragma once

class Computer
{
public:
	Computer();
	struct Buffer
	{
		Buffer()														   : data(NULL), pointer(NULL), readFlag(false), byteSize(0) {};
		Buffer(cl_mem data, void* pointer, bool readFlag, size_t byteSize) : data(data), pointer(pointer), readFlag(readFlag), byteSize(byteSize) {};
		cl_mem data;
		void* pointer;
		bool readFlag;
		size_t byteSize;
		~Buffer() {};
	};

	typedef std::vector<Buffer> Buffers;
	//
	void set_Device(Device device);
	void set_Kernel_Source(std::string kernel_Source);
	void set_Kernel_Name(std::string kernel_Name);
	void set_nThread(size_t nThread);
	void set_nThread_in_Group(size_t nThread_in_Group);
	ErrCode build_kernel();
	template< typename Type > ErrCode add_In(std::vector<Type>& arg);
	template< typename Type > ErrCode add_Out(std::vector<Type>& arg);
	template< typename Type > ErrCode add_InOut(std::vector<Type>& arg);
	template< typename Type > ErrCode add_Constant(Type arg);
	ErrCode add_Local(size_t byteSize);
	ErrCode compute();
	ErrCode clear();
	double get_buildTime();
	double get_computeTime();
	~Computer();
private:
	Device device;
	std::string kernel_Source;
	std::string kernel_Name;
	size_t nThread;
	size_t nThread_in_Group;
	//
	int nArg;
	double buildTime;
	double computeTime;
	//
	Buffers buffers;
	//
	cl_context context_;
	cl_command_queue queue_;
	cl_program program_;
	cl_kernel kernel_;
	//
};

template< typename Type > ErrCode Computer::add_In(std::vector<Type>& arg)
{
	ErrCode err = 0;
	size_t byteSize = sizeof(Type)*arg.size();

	cl_mem mem = clCreateBuffer(context_, CL_MEM_READ_ONLY, byteSize, NULL, &err);
	if (err != 0) return err;

	err = clEnqueueWriteBuffer(queue_, mem, CL_TRUE, 0, byteSize, &arg[0], 0, NULL, NULL);
	if (err != 0) return err;

	buffers.push_back(Buffer(mem, &arg[0], false, byteSize));

	err = clSetKernelArg(kernel_, nArg, sizeof(cl_mem), &buffers.back());

	if (err == 0)  ++nArg;

	return err;
}


template< typename Type > ErrCode Computer::add_Out(std::vector<Type>& arg)
{
	ErrCode err = 0;
	size_t byteSize = sizeof(Type)*arg.size();

	cl_mem mem = clCreateBuffer(context_, CL_MEM_WRITE_ONLY, byteSize, NULL, &err);
	if (err != 0) return err;

	buffers.push_back(Buffer(mem, &arg[0], true, byteSize));

	err = clSetKernelArg(kernel_, nArg, sizeof(cl_mem), &buffers.back());

	if (err == 0)  ++nArg;

	return err;
}

template< typename Type > ErrCode Computer::add_InOut(std::vector<Type>& arg)
{
	ErrCode err = 0;
	size_t byteSize = sizeof(Type)*arg.size();

	cl_mem mem = clCreateBuffer(context_, CL_MEM_READ_WRITE, byteSize, NULL, &err);
	if (err != 0) return err;

	err = clEnqueueWriteBuffer(queue_, mem, CL_TRUE, 0, byteSize, &arg[0], 0, NULL, NULL);
	if (err != 0) return err;

	buffers.push_back(Buffer(mem, &arg[0], true, byteSize));

	err = clSetKernelArg(kernel_, nArg, sizeof(cl_mem), &buffers.back());

	if (err == 0)  ++nArg;

	return err;
}


template< typename Type > ErrCode Computer::add_Constant(Type arg)
{
	ErrCode err = 0;
	size_t arg_byteSize = sizeof(Type);
	err = clSetKernelArg(kernel_, nArg, arg_byteSize, (void*)&arg);
	if (err == 0)  ++nArg;
	return err;
}

