#include<assoc.h>
#include<clutil.h>


void
gpuAssoc()
{
    initExecution();

    // Assoc code goes ehre


    printGpuTime();
    cleanup();
}




void
test()
{
    // setting up opencl devices
    initExecution();
    const unsigned sizeData = sizeof(int) * SIZE;
    int* a = (int*)allocateHostMemory(sizeData);
    int* b = (int*)allocateHostMemory(sizeData);        
    int* c = (int*)allocateHostMemory(sizeData);
    for (unsigned i = 0; i < SIZE; ++i) {
        a[i] = 1;
        b[i] = 1;
    }
    // allocating device memory
    // copies to device as well if first arg is not null
    cl_mem dA = allocateDeviceMemory(a, sizeData, CL_MEM_READ_ONLY); 
    cl_mem dB = allocateDeviceMemory(b, sizeData, CL_MEM_READ_ONLY); 
    cl_mem dC = allocateDeviceMemory(c, sizeData, CL_MEM_WRITE_ONLY);

    // compiling the kernel source
    compileProgram(0, "kernel.cl");
    // creating kernel
    createKernel("vecAdd");
    // setting arguments to kernel
    setKernelArg("vecAdd", 0,  sizeof(cl_mem), &dA);
    setKernelArg("vecAdd", 1, sizeof(cl_mem), &dB);
    setKernelArg("vecAdd", 2, sizeof(cl_mem), &dC);
    size_t szLocalWorkSize = NO_THREADS_BLOCK;
    size_t szGlobalWorkSize = SIZE;
    // running kernel
    runKernel("vecAdd", szLocalWorkSize, szGlobalWorkSize);
    waitForEvent();
    // copy result from device
    copyFromDevice(dC, c, sizeData);
    for (unsigned i = 0; i < SIZE; ++i) {
        if (c[i] != 2 ) {
            cout << "error" << endl;
            break;
        }
    }
    printGpuTime();
    cleanup();
}
