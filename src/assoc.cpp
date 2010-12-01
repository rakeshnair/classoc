#include<assoc.h>
#include<clutil.h>
#include<assert.h>


void
gpuAssoc(const char* file)
{
    fileRead(file);
    initExecution();
    int* count = (int*)allocateHostMemory(nItems * sizeof(int));
    for (unsigned i = 0; i < nItems; ++i) {
        count[i] = 0;
    }
    const int marketBasketSize = sizeof(bool) * nItems * nTransactions;
    cl_mem dMarketBasket = allocateDeviceMemory(marketBasket, marketBasketSize,
                                                CL_MEM_READ_ONLY);
    cl_mem dCount = allocateDeviceMemory(count, nItems * sizeof(int),
                                                CL_MEM_WRITE_ONLY);
    compileProgram(0, "kernel.cl");
    createKernel("itemCount");
    setKernelArg("itemCount", 0, sizeof(cl_mem), &dMarketBasket); 
    setKernelArg("itemCount", 1, sizeof(cl_mem), &dCount); 
    setKernelArg("itemCount", 2, sizeof(unsigned), (void*)&nTransactions); 
    setKernelArg("itemCount", 3, sizeof(unsigned), (void*)&nItems); 

    size_t szLocalWorkSize = NO_THREADS_BLOCK;
    size_t szGlobalWorkSize;
    if (nItems % 256 == 0) { 
        szGlobalWorkSize = nItems;
    } else {
        szGlobalWorkSize = 256 * ((nItems / 256) + 1);

    }
    
    runKernel("itemCount", szLocalWorkSize, szGlobalWorkSize);


    waitForEvent();
    copyFromDevice(dCount, count, nItems * sizeof(int));


    // Assoc code goes ehre

    printGpuTime();
    verifyCount(count);
    cleanup();
}

void 
verifyCount(int * count)
{
	for (unsigned k = 0; k < nItems; ++k) {
        unsigned countI = 0;
		for (unsigned i = 0; i < nTransactions; ++i ) {
			if (marketBasket[k * nTransactions + i] == 1) {
				//    cout << i + 1 << endl;
				++countI;
			}
		}
        if (countI != count[k]) {
           cout << "**********ERROR in item " << k <<  endl;
           return;
        } 
	}
    cout << "Verified" << endl;
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
