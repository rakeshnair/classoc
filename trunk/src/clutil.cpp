/* 
 File: clutil.c 
 Functions to help with opencl management.
*/
#include <clutil.h>
#include<vector>
#include<assert.h>
#include<string.h>
#include<fstream>
#include<map>
#include<string>


using namespace std;
// global variables
cl_context cxContext = 0;
cl_program cpProgram = 0;
cl_command_queue commandQueue;
vector<cl_event> gpuExecution;
cl_device_type device_type;
vector<cl_mem> deviceBuffers;
vector<void*> hostBuffers;
cl_device_id device;
typedef map<string, cl_kernel> NameToKernel;
NameToKernel nameToKernel;

unsigned
initExecution()
{

#ifdef CPU
    device_type = CL_DEVICE_TYPE_CPU;
    cout << "CPU Execution" << endl;
#else
    device_type = CL_DEVICE_TYPE_GPU;
    cout << "GPU Execution" << endl;
#endif

    cout << "Initializing device(s).." << endl;
    // create the OpenCL context on available GPU devices
    init_cl_context();

    const cl_uint ciDeviceCount =  getDeviceCount();


    if (!ciDeviceCount) {
        printf("No opencl specific devices!\n");
		return 0;
	}

	printf("Creating Command Queue...\n");
	createCommandQueue();
	return 1;
}

void
printGpuTime()
{
    cout << "Kernel execution Time on " 
         << ((device_type == CL_DEVICE_TYPE_GPU ) ? "GPU" : "CPU")
         << " : " << executionTime() << " ms" << endl << endl;
}


double 
executionTime()
{
    cl_ulong time = 0;
	for (unsigned i = 0; i < gpuExecution.size(); ++i) {
		cl_ulong start, end;
		clGetEventProfilingInfo(gpuExecution[i], 
				CL_PROFILING_COMMAND_END, 
				sizeof(cl_ulong), &end, NULL);
		clGetEventProfilingInfo(gpuExecution[i],
				CL_PROFILING_COMMAND_START, 
				sizeof(cl_ulong), &start, NULL);
        time += (end - start);
	}
    double sec = (1.0e-9 * time) * 1000; 
    return sec; 
}

/* size in bytes*/
cl_mem
allocateDeviceMemory(void* buffer, const unsigned size, 
                     cl_mem_flags flags)
                                      
{
    cl_int ciErrNum = CL_SUCCESS;
    cl_mem dBuffer = clCreateBuffer(cxContext,
                                  flags,
                                  size,
                                  0,
                                  &ciErrNum);
    checkError(ciErrNum, "clCreateBuffer");
    deviceBuffers.push_back(dBuffer);

    if (buffer) {
    	copyToDevice(dBuffer, buffer, size);
    }

    return dBuffer;
}

void*
allocateHostMemory(const unsigned size)
{
    void* p = malloc(size);
    memset(p, 0, size);
    hostBuffers.push_back(p);
    return p;
}

void
freeDeviceBuffer(cl_mem db)
{
	if (db) {
		clReleaseMemObject(db);
	}
}


void 
cleanup()
{
	// clean up device
    for (unsigned i = 0; i < deviceBuffers.size(); ++i) {
    	cl_mem mem = deviceBuffers[i];
        if (mem) {
             clReleaseMemObject(mem);
        } 
    }

    deviceBuffers.clear();

	// cleanup ocl routines
    for (unsigned i = 0; i < gpuExecution.size(); ++i) {
        if (gpuExecution[i]) {
            clReleaseEvent(gpuExecution[i]);
        }
    }
    gpuExecution.clear();
   
    NameToKernel::const_iterator itr = nameToKernel.begin();
    for (; itr != nameToKernel.end(); ++itr) {
        cl_kernel k = (*itr).second;
	 	clReleaseKernel(k);
    }

    for (unsigned i = 0; i < hostBuffers.size(); ++i) {
         free(hostBuffers[i]);             
    }
    hostBuffers.clear();

	if (commandQueue) {
		clReleaseCommandQueue(commandQueue);
	}

	if (cpProgram) {
		clReleaseProgram(cpProgram);
	}
	if (cxContext) { 
		// segfaults if context is null.
		clReleaseContext(cxContext);
	}
}

void
checkError(const cl_int ciErrNum, const char* const operation) 
{
    if (ciErrNum != CL_SUCCESS) {
        cout << "ERROR:: " << operation << " failed "
             << oclErrorString(ciErrNum) << endl; 
        cleanup();
        exit(EXIT_FAILURE);
    }    
}

void
init_cl_context()
{
    cl_int ciErrNum = CL_SUCCESS; 
    cl_platform_id cpPlatform;
    
#if 0
    cxContext = clCreateContextFromType(0, /* cl_context_properties */
                                        device_type,
                                        NULL, /* error function ptr */
                                        NULL, /* user data to be passed to err fn */
                                        &ciErrNum)
#endif
    ciErrNum = clGetPlatformIDs(1, &cpPlatform, NULL);
    checkError(ciErrNum, "clGetPlatformIDs");

    ciErrNum = clGetDeviceIDs(cpPlatform, device_type, 1, 
                              &device, NULL);
    
    checkError(ciErrNum, "clGetDeviceIDs");
    cxContext = clCreateContext(0, 1, &device, NULL, NULL, &ciErrNum); 

    checkError(ciErrNum, "clCreateContext");
}

cl_uint 
getDeviceCount()
{
    size_t nDeviceBytes;
  
    const cl_int ciErrNum = clGetContextInfo(cxContext,
                              CL_CONTEXT_DEVICES, /* Param Name */
                              0, /* size_t param_value_size  */
                              NULL, /* void * param_value */
                              &nDeviceBytes); /* size_t param_value_size_ret */
  
    checkError(ciErrNum, "clGetContextInfo");
    return ((cl_uint)nDeviceBytes/sizeof(cl_device_id));
}
      

void
createCommandQueue()
{
    cl_int ciErrNum = CL_SUCCESS;
    commandQueue  = clCreateCommandQueue(cxContext, 
                                         device,
                                         CL_QUEUE_PROFILING_ENABLE,
                                         &ciErrNum);
    checkError(ciErrNum, "clCreateCommandQueue"); 
}                                     


void
compileProgram(const char* const header_file, 
               const char* const kernel_file)
{

 
    size_t program_length;
    char* source = readFile(header_file, kernel_file, program_length);

    cl_int ciErrNum; 
    // Create the program for all GPUs in the context
    cpProgram = clCreateProgramWithSource( cxContext, 1,
                                           (const char **) &source,
                                           &program_length,
                                           &ciErrNum);
    //free(header);
    free(source);
     
    checkError(ciErrNum, "clCreateProgramWithSource"); 
    /* Build program */

    ciErrNum = clBuildProgram(cpProgram,
                              1,        /* Number of devices for which we need to do this */
                              &device, /* Device List */
                              "-DKERNEL",
                              NULL, /* ptr to function */
                              NULL); /* User data to pass to ptrfn */

    if (ciErrNum != CL_SUCCESS) {
        buildError();
        checkError(ciErrNum, "clBuildProgram");
    }
}

char*
readFile(const char* headerFileName, const char* srcFileName, 
         size_t& length)
{
	ifstream head;
	size_t hLength = 0;
	if (headerFileName) {
		head.open(headerFileName, ios_base::in);
		if (!head.is_open()) {
			cout << "File " << headerFileName << " cannot be opened" << endl;
			cleanup();
			exit(EXIT_FAILURE);
		}
		head.seekg (0, ios::end);
		hLength = head.tellg();
		head.seekg (0, ios::beg);
	}
	assert(srcFileName);
	ifstream src;
	size_t sLength = 0;
	src.open(srcFileName, ios_base::in);
	if (!src.is_open()) {
		cout << "File " << srcFileName << " cannot be opened" << endl;
		cleanup();
		exit(EXIT_FAILURE);
	}
	src.seekg (0, ios::end);
	sLength = src.tellg();
	src.seekg (0, ios::beg);
    
    length = hLength + sLength;
    assert(length);

    char* buffer = (char*)malloc(length);
	if (headerFileName) {
		head.read(buffer, hLength);
		head.close();
	}
	src.read(buffer + hLength, sLength);
    src.close(); 
    return buffer;
}

void
buildError()
{
    char *buildLog = NULL;
    size_t buildLogSize = 0;
    cl_int ciErrNum = clGetProgramBuildInfo (cpProgram,
            device,
            CL_PROGRAM_BUILD_LOG,
            buildLogSize,
            buildLog,
            &buildLogSize);
    checkError(ciErrNum, "clBuildProgramInfo");
    buildLog = (char*)malloc(buildLogSize);
    if(buildLog == NULL)
    {
        cout << "ERROR:: Not enough memory" << endl;
        cleanup();
        exit(EXIT_FAILURE);
    }
    memset(buildLog, 0, buildLogSize);
    ciErrNum = clGetProgramBuildInfo (cpProgram,
            device,
            CL_PROGRAM_BUILD_LOG,
            buildLogSize,
            buildLog,
            NULL);
    checkError(ciErrNum, "clBuildProgramInfo");
    std::cout << " \n\t\t\tBUILD LOG\n";
    std::cout << " ************************************************\n";
    std::cout << buildLog << std::endl;
    std::cout << " ************************************************\n";
    free(buildLog);
}



void
createKernel(const string& kernelName)
{  
    assert(nameToKernel.find(kernelName) == nameToKernel.end());
    cl_int ciErrNum = CL_SUCCESS;
    cl_kernel k = clCreateKernel(cpProgram, kernelName.c_str(), &ciErrNum);
    checkError(ciErrNum, "clCreateKernel");
    nameToKernel[kernelName] = k;
    assert(nameToKernel.find(kernelName) != nameToKernel.end());
    //nameToKernel.insert(kernelName, temp);
      
}

cl_mem 
createDeviceBuffer(const cl_mem_flags flags, 
                   const size_t size, void* const hostPtr)
                   


{
    cl_int ciErrNum = CL_SUCCESS;
    const cl_mem d_mem = clCreateBuffer(cxContext,
                                        flags,
                                        size,
                                        hostPtr,
                                        &ciErrNum);
  
    checkError(ciErrNum, "clCreateBuffer");
    return d_mem;
}

void 
copyToDevice(const cl_mem mem, void* const hostPtr, 
             const unsigned size)
{
    const cl_int ciErrNum = clEnqueueWriteBuffer(commandQueue, 
                                                mem, CL_TRUE, 0, 
                                                size, hostPtr, 
                                                0, NULL, NULL);
    checkError(ciErrNum, "clEnqueueWriteBuffer");
}

void
copyFromDevice(const cl_mem dMem, void* hostPtr, 
               const unsigned size)
{
    cl_int ciErrNum = clEnqueueReadBuffer(commandQueue, dMem,
                                          CL_TRUE, 0,
                                          size,
                                          hostPtr, 0, NULL, 
                                          NULL);
    checkError(ciErrNum, "clEnqueueReadBuffer"); 
}


void 
runKernel(const string& kernel, size_t localWorkSize, 
          size_t globalWorkSize)
{
    NameToKernel::const_iterator itr = nameToKernel.find(kernel);
    assert(itr != nameToKernel.end());
#ifdef DEBUG 
    print(kernel, localWorkSize, globalWorkSize);
#endif
    cl_kernel k = (*itr).second;
    cl_event e;
    const cl_int ciErrNum = clEnqueueNDRangeKernel(commandQueue, 
                                                   k, // kernel 
                                                   1, NULL, 
                                                   &globalWorkSize, 
                                                   &localWorkSize, 0, 
                                                   NULL, 
                                                   &e);
    checkError(ciErrNum, "clEnqueueNDRangeKernel");
    gpuExecution.push_back(e);
}

void
setKernelArg(const string& kernel, const unsigned argIndex,  
             const size_t argSize, const void* argVal)
{
    NameToKernel::iterator itr = nameToKernel.find(kernel);
    assert(itr != nameToKernel.end()); 
    cl_kernel k = (*itr).second; 
    cl_int ciErrNum = clSetKernelArg(k, argIndex ,
                                     argSize, argVal);
    checkError(ciErrNum, "clSetKernelArg");
}


void 
waitForEvent()
{
    if (gpuExecution.size() == 0) return;
    cl_int ciErrNum = clWaitForEvents(1, &gpuExecution[gpuExecution.size() - 1]);
    checkError(ciErrNum, "clWaitForEvents");
}

void
print(const string& kernel, const size_t szLocalWorkSize, 
                            const size_t szGlobalWorkSize)
{
    cout << "Kernel: " <<  kernel << endl
         << "Global Work Size " << szGlobalWorkSize << endl
         << "Local Work Size " << szLocalWorkSize << endl
         << "# of Work Groups " << (szGlobalWorkSize % szLocalWorkSize 
                                   + szGlobalWorkSize/szLocalWorkSize)
         << endl;
}

const char* oclErrorString(cl_int error)
{
    static const char* errorstring[] = {
        "cl_success",
        "cl_device_not_found",
        "cl_device_not_available",
        "cl_compiler_not_available",
        "cl_mem_object_allocation_failure",
        "cl_out_of_resources",
        "cl_out_of_host_memory",
        "cl_profiling_info_not_available",
        "cl_mem_copy_overlap",
        "cl_image_format_mismatch",
        "cl_image_format_not_supported",
        "cl_build_program_failure",
        "cl_map_failure",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "cl_invalid_value",
        "cl_invalid_device_type",
        "cl_invalid_platform",
        "cl_invalid_device",
        "cl_invalid_context",
        "cl_invalid_queue_properties",
        "cl_invalid_command_queue",
        "cl_invalid_host_ptr",
        "cl_invalid_mem_object",
        "cl_invalid_image_format_descriptor",
        "cl_invalid_image_size",
        "cl_invalid_sampler",
        "cl_invalid_binary",
        "cl_invalid_build_options",
        "cl_invalid_program",
        "cl_invalid_program_executable",
        "cl_invalid_kernel_name",
        "cl_invalid_kernel_definition",
        "cl_invalid_kernel",
        "cl_invalid_arg_index",
        "cl_invalid_arg_value",
        "cl_invalid_arg_size",
        "cl_invalid_kernel_args",
        "cl_invalid_work_dimension",
        "cl_invalid_work_group_size",
        "cl_invalid_work_item_size",
        "cl_invalid_global_offset",
        "cl_invalid_event_wait_list",
        "cl_invalid_event",
        "cl_invalid_operation",
        "cl_invalid_gl_object",
        "cl_invalid_buffer_size",
        "cl_invalid_mip_level",
        "cl_invalid_global_work_size",
    };

    const int errorcount = sizeof(errorstring) / sizeof(errorstring[0]);
    const int index = -error;
    return (index >= 0 && index < errorcount) ? errorstring[index] : "";
}
                                                                          

