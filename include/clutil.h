#ifndef __CLUTIL__
#define __CLUTIL__

#include <CL/cl.h>
#include<iostream>
#include <sys/time.h>
#include<sys/resource.h>
#include <sys/types.h>
#include<string>
using namespace std;


unsigned initExecution();
void waitForEvent();
void print(const string&, const size_t, const size_t);

void checkError(const cl_int ciErrNum, const char* const operation);
const char* oclErrorString(cl_int error);

void verifyResult(const unsigned size);

cl_kernel getKernel(const unsigned index);

void init_cl_context();

cl_uint getDeviceCount();

void createCommandQueue();
void
setKernelArg(const string& kernel, const unsigned argIndex,
             const size_t argSize, const void* argVal);


void buildError();
char*
readFile(const char* headerFileName, const char* srcFileName,  
                   size_t& length);

void compileProgram(const char* const header_file,
                    const char* const kernel_file);
void* allocateHostMemory(const unsigned size);


void createKernel(const string&);


void runKernel(const string&, const size_t localWorkSize,
               const size_t globalWorkSize);
void copyToDevice(const cl_mem mem,
                  void* const hostPtr, const unsigned size);


void copyFromDevice(const cl_mem dMem,
                    void* hostPtr, const unsigned size);

double executionTime();
cl_mem allocateDeviceMemory(void* buffer, const unsigned size, cl_mem_flags);

void freeDeviceBuffer(cl_mem);

void printGpuTime();

void cleanup();


#endif


