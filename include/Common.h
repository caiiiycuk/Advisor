/****
* Назначение:
* чтение файлов исторических данных
****/

#pragma once

// *******************************************Константы
#define MIN_RATING -10000
#define N_BIT_IN_CHROMOSECTOR (sizeof(ChromoSector) * 8)
#define KERNEL_FILE_SUFFIX "_"

// *******************************************Элементарные типы
typedef int ErrCode;
typedef unsigned int ChromoSector;				// Сектор хромосом
typedef std::vector<ChromoSector> ChromoSectors;// Множество секторов
typedef std::vector<ChromoSector> ChromoSet;	// Набор хромосом (соответствует одной особи)
typedef std::vector<ChromoSet> ChromoSets;		// Множество наборов хромосом
typedef std::vector<std::string> Strings;

// *******************************************Структурные типы
struct History
{
	History()														 : nPoint(-1), name("") {};
	History(int nPoint)												 : nPoint(nPoint), name(""), tms(nPoint), pricesOpen(nPoint), pricesClose(nPoint), pricesLow(nPoint), pricesHigh(nPoint), values(nPoint){};
	int nPoint;
	std::string name;
	std::vector<tm> tms;
	std::vector<float> pricesOpen;
	std::vector<float> pricesClose;
	std::vector<float> pricesLow;
	std::vector<float> pricesHigh;
	std::vector<float> values;
	~History() {};
};

struct SampleIndividual
{
	SampleIndividual()												 : c1(0), c0(0), rating(MIN_RATING), nBit_in_Chromoset(-1), nByte_in_Chromoset(-1), nChromoSector_in_Chromoset(-1){};
	SampleIndividual(int nChromoSector)								 : c1(0), c0(0), rating(MIN_RATING), nBit_in_Chromoset(-1), nByte_in_Chromoset(-1), nChromoSector_in_Chromoset(nChromoSector),
																	   chromoSectors(nChromoSector), invertChromoSectors(nChromoSector)	{};
	ChromoSectors chromoSectors;
	ChromoSectors invertChromoSectors;
	int c1;
	int c0;
	int rating;
	int nBit_in_Chromoset;
	int nByte_in_Chromoset;
	int nChromoSector_in_Chromoset;
	~SampleIndividual() {};
};

struct Individual
{
	Individual()													 : expression(""), c11(0), c01(0), rating(MIN_RATING), certified(false) {};
	Individual(int nChromoSector)									 : expression(""), c11(0), c01(0), rating(MIN_RATING), certified(false), chromoSectors(nChromoSector) {};
	Individual(std::string expression)								 : expression(expression), c11(0), c01(0), rating(MIN_RATING), certified(false) {};
	Individual(std::string expression, int nChromoSector)			 : expression(expression), c11(0), c01(0), rating(MIN_RATING), certified(false), chromoSectors(nChromoSector) {};
	Individual(std::string expression, int c11, int c01, int rating) : expression(expression), c11(c11), c01(c01), rating(rating), certified(false) {};
	ChromoSectors chromoSectors;
	std::string expression;
	int c11;
	int c01;
	int rating;
	bool certified;
	~Individual() {};
};

typedef std::multimap<int, Individual, std::less<int> > Individuals;

struct Device
{
	Device() {};
	cl_device_id id = 0;

	// Данные, собираемые функцией clGetDeviceInfo(..)
	cl_uint address_bits;
	cl_bool	available;
	cl_bool	compiler_available;
	cl_device_fp_config	double_fp_config;
	cl_bool	endian_little;
	cl_bool	error_correction_support;
	cl_device_exec_capabilities	execution_capabilities;
	std::string extensions;
	cl_ulong	global_mem_cache_size;
	cl_device_mem_cache_type	global_mem_cache_type;
	cl_ulong	global_mem_cacheline_size;
	cl_uint	global_mem_size;
	cl_bool	image_support;
	size_t	image2d_max_height;
	size_t	image2d_max_width;
	size_t	image3d_max_depth;
	size_t	image3d_max_height;
	size_t	image3d_max_width;
	cl_ulong	local_mem_size;
	cl_device_local_mem_type	local_mem_type;
	cl_uint	max_clock_frequency;
	cl_uint	max_compute_units;
	cl_uint	max_constant_args;
	cl_ulong max_constant_buffer_size;
	cl_ulong max_mem_alloc_size;
	size_t	max_parameter_size;
	cl_uint	max_read_image_args;
	cl_uint	max_samplers;
	size_t	max_work_group_size;
	cl_uint	max_work_item_dimensions;
	std::vector<size_t>	max_work_item_sizes;
	cl_uint	max_write_image_args;
	cl_uint	mem_base_addr_align;
	cl_uint	min_data_type_align_size;
	std::string	name;
	cl_uint	preferred_vector_width_char;
	cl_uint	preferred_vector_width_double;
	cl_uint	preferred_vector_width_float;
	cl_uint	preferred_vector_width_int;
	cl_uint preferred_vector_width_long;
	cl_uint preferred_vector_width_short;
	std::string profile;
	size_t	profiling_timer_resolution;
	cl_command_queue_properties	queue_properties;
	cl_device_fp_config	single_fp_config;
	cl_device_type	type;
	std::string	vendor;
	cl_uint	vendor_id;
	std::string	version;
	std::string	driver_version;

	// Дополнительные
	size_t LDS_ByteSize;
	size_t reg_ByteSize;
	size_t nReg_in_CU;
	size_t nMaxWarp_in_CU;
	size_t nMaxThread_in_Warp;
	~Device() {};
};

typedef std::vector<Device> Devices;

struct Platform
{
	Platform() :													 id(NULL), profile(""), version(""), name(""), vendor(""), extensions(""){};
	Devices devices;
	cl_platform_id id;

	// Данные, собираемые функцией clGetPlatformInfo(..)
	std::string profile;
	std::string version;
	std::string name;
	std::string vendor;
	std::string extensions;
	~Platform() {};
};

typedef std::vector<Platform> Platforms;


