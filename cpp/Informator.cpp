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
#include <map>
#include "Common.h"
#include "Informator.h"


Informator::Informator()
{
}


ErrCode Informator::get_Platforms(Platforms& platforms)
{
	ErrCode err = 0;
	cl_uint nPlatform = 0;

	// Определяем количество платформ
	err = clGetPlatformIDs(0, NULL, &nPlatform);

	// Создаем локальный (темповый) вектор платформ
	std::vector<cl_platform_id> platformIds(nPlatform);

	// Изменяем размерность результирующего вектора 
	platforms.resize(nPlatform);

	// Считываем идентификаторы флатформ в локальный (темповый) вектор
	err = clGetPlatformIDs(nPlatform, &(platformIds[0]), NULL);

	for (int i = 0; i < nPlatform; ++i)
	{
		// Заполняем идентификаторы платформ в результирующий вектор
		Platform& platform = platforms[i];
		platform.id = platformIds[i];

		const int bufferSize = 10240;
		char buffer[bufferSize];

		// profile
		err = clGetPlatformInfo(platform.id, CL_PLATFORM_PROFILE, bufferSize, buffer, NULL);
		platform.profile = std::string(buffer);

		// version
		err = clGetPlatformInfo(platform.id, CL_PLATFORM_VERSION, bufferSize, buffer, NULL);
		platform.version = std::string(buffer);

		// name
		err = clGetPlatformInfo(platform.id, CL_PLATFORM_NAME, bufferSize, buffer, NULL);
		platform.name = std::string(buffer);

		// vendor
		err = clGetPlatformInfo(platform.id, CL_PLATFORM_VENDOR, bufferSize, buffer, NULL);
		platform.vendor = std::string(buffer);

		// extensions
		err = clGetPlatformInfo(platform.id, CL_PLATFORM_EXTENSIONS, bufferSize, buffer, NULL);
		platform.extensions = std::string(buffer);

		// Определяем количество устройств
		cl_uint nDevices;
		err = clGetDeviceIDs(platform.id, CL_DEVICE_TYPE_ALL, 0, NULL, &nDevices);

		// Создаем локальный (темповый) вектор устройств
		std::vector<cl_device_id> deviceIds(nDevices);

		// Изменяем размерность результирующего вектора 
		platform.devices.resize(nDevices);

		// Считываем идентификаторы устройств в локальный (темповый) вектор
		err = clGetDeviceIDs(platform.id, CL_DEVICE_TYPE_ALL, nDevices, &deviceIds[0], NULL);

		for (int j = 0; j < nDevices; ++j)
		{
			// Заполняем идентификаторы платформ в результирующий вектор
			Device& device = platform.devices[j];
			device.id = deviceIds[j];

			// address_bits
			err = clGetDeviceInfo(device.id, CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &device.address_bits, NULL);

			// available
			err = clGetDeviceInfo(device.id, CL_DEVICE_AVAILABLE, sizeof(cl_bool), &device.available, NULL);

			// compiler_available
			err = clGetDeviceInfo(device.id, CL_DEVICE_COMPILER_AVAILABLE, sizeof(cl_bool), &device.compiler_available, NULL);

			// double_fp_config
			err = clGetDeviceInfo(device.id, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(cl_device_fp_config), &device.double_fp_config, NULL);

			// endian_little
			err = clGetDeviceInfo(device.id, CL_DEVICE_ENDIAN_LITTLE, sizeof(cl_bool), &device.endian_little, NULL);

			// error_correction_support
			err = clGetDeviceInfo(device.id, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(cl_bool), &device.error_correction_support, NULL);

			// execution_capabilities
			err = clGetDeviceInfo(device.id, CL_DEVICE_EXECUTION_CAPABILITIES, sizeof(cl_device_exec_capabilities), &device.execution_capabilities, NULL);

			// extensions
			err = clGetDeviceInfo(device.id, CL_DEVICE_EXTENSIONS, bufferSize, buffer, NULL);
			device.extensions = std::string(buffer);

			// global_mem_cache_size
			err = clGetDeviceInfo(device.id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &device.global_mem_cache_size, NULL);

			// global_mem_cache_type
			err = clGetDeviceInfo(device.id, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cl_device_mem_cache_type), &device.global_mem_cache_type, NULL);

			// global_mem_cacheline_size
			err = clGetDeviceInfo(device.id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint), &device.global_mem_cacheline_size, NULL);

			// global_mem_size
			err = clGetDeviceInfo(device.id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &device.global_mem_size, NULL);

			// image_support
			err = clGetDeviceInfo(device.id, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &device.image_support, NULL);

			// image_image2d_max_height
			err = clGetDeviceInfo(device.id, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &device.image2d_max_height, NULL);

			// image2d_max_width
			err = clGetDeviceInfo(device.id, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &device.image2d_max_width, NULL);

			// image3d_max_depth
			err = clGetDeviceInfo(device.id, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &device.image3d_max_depth, NULL);

			// image3d_max_height
			err = clGetDeviceInfo(device.id, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &device.image3d_max_height, NULL);

			// image3d_max_width
			err = clGetDeviceInfo(device.id, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &device.image3d_max_width, NULL);

			// local_mem_size
			err = clGetDeviceInfo(device.id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &device.local_mem_size, NULL);

			// local_mem_type
			err = clGetDeviceInfo(device.id, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), &device.local_mem_type, NULL);

			// max_clock_frequency
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &device.max_clock_frequency, NULL);

			// max_compute_units
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &device.max_compute_units, NULL);

			// max_constant_args
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(cl_uint), &device.max_constant_args, NULL);

			// max_constant_buffer_size
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &device.max_constant_buffer_size, NULL);

			// max_mem_alloc_size
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &device.max_mem_alloc_size, NULL);

			// max_parameter_size
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(size_t), &device.max_parameter_size, NULL);

			// max_read_image_args
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(cl_uint), &device.max_read_image_args, NULL);

			// max_samplers
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_SAMPLERS, sizeof(cl_uint), &device.max_samplers, NULL);

			// max_samplers
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &device.max_work_group_size, NULL);

			// max_samplers
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &device.max_work_item_dimensions, NULL);

			// max_work_item_sizes
			device.max_work_item_sizes.resize(device.max_work_item_dimensions);
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, device.max_work_item_dimensions*sizeof(size_t), &device.max_work_item_sizes[0], NULL);

			// max_write_image_args
			err = clGetDeviceInfo(device.id, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(cl_uint), &device.max_write_image_args, NULL);

			// mem_base_addr_align
			err = clGetDeviceInfo(device.id, CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(cl_uint), &device.mem_base_addr_align, NULL);

			// min_data_type_align_size
			err = clGetDeviceInfo(device.id, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, sizeof(cl_uint), &device.min_data_type_align_size, NULL);

			// name
			err = clGetDeviceInfo(device.id, CL_DEVICE_NAME, bufferSize, buffer, NULL);
			device.name = std::string(buffer);

			// preferred_vector_width_char
			err = clGetDeviceInfo(device.id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_uint), &device.preferred_vector_width_char, NULL);

			// preferred_vector_width_double
			err = clGetDeviceInfo(device.id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &device.preferred_vector_width_double, NULL);

			// preferred_vector_width_float
			err = clGetDeviceInfo(device.id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &device.preferred_vector_width_float, NULL);

			// preferred_vector_width_int
			err = clGetDeviceInfo(device.id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &device.preferred_vector_width_int, NULL);

			// preferred_vector_width_long
			err = clGetDeviceInfo(device.id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &device.preferred_vector_width_long, NULL);

			// preferred_vector_width_short
			err = clGetDeviceInfo(device.id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), &device.preferred_vector_width_short, NULL);

			// profile
			err = clGetDeviceInfo(device.id, CL_DEVICE_PROFILE, bufferSize, buffer, NULL);
			device.profile = std::string(buffer);

			// profiling_timer_resolution
			err = clGetDeviceInfo(device.id, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(size_t), &device.profiling_timer_resolution, NULL);

			// queue_properties
			err = clGetDeviceInfo(device.id, CL_DEVICE_QUEUE_PROPERTIES, sizeof(size_t), &device.queue_properties, NULL);

			// single_fp_config
			err = clGetDeviceInfo(device.id, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(cl_device_fp_config), &device.single_fp_config, NULL);

			// type
			err = clGetDeviceInfo(device.id, CL_DEVICE_TYPE, sizeof(cl_device_type), &device.type, NULL);

			// vendor
			err = clGetDeviceInfo(device.id, CL_DEVICE_VENDOR, bufferSize, buffer, NULL);
			device.vendor = std::string(buffer);

			// vendor_id
			err = clGetDeviceInfo(device.id, CL_DEVICE_VENDOR_ID, sizeof(cl_uint), &device.vendor_id, NULL);

			// version
			err = clGetDeviceInfo(device.id, CL_DEVICE_VERSION, bufferSize, buffer, NULL);
			device.version = std::string(buffer);

			// driver_version
			err = clGetDeviceInfo(device.id, CL_DRIVER_VERSION, bufferSize, buffer, NULL);
			device.driver_version = std::string(buffer);

			device.LDS_ByteSize = 0;
			device.reg_ByteSize = 0;
			device.nReg_in_CU = 0;
			device.nMaxWarp_in_CU = 0;
			device.nMaxThread_in_Warp = 0;
		}
	}
	return 0;
}


ErrCode Informator::get_Device_with_max_nCU(Device& device, const Platforms& platforms)
{
	ErrCode err = 0;
	device.max_compute_units = 0;

	cl_uint nPlatform = platforms.size();

	for (int i = 0; i < nPlatform; ++i)
	{
		const Platform& platform = platforms[i];
		cl_uint nDevices = platform.devices.size();

		for (int j = 0; j < nDevices; ++j)
		{
			if (platform.devices[j].max_compute_units > device.max_compute_units)
			{
				device = platform.devices[j];
			}
		}

	}
	return 0;
}


Informator::~Informator()
{
}
