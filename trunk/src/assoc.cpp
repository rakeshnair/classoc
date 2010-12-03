#include<assoc.h>
#include<clutil.h>
#include<assert.h>
#include<bitset>


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
#if 0
    createKernel("itemCount1");
    setKernelArg("itemCount1", 0, sizeof(cl_mem), &dMarketBasket); 
    setKernelArg("itemCount1", 1, sizeof(cl_mem), &dCount); 
    setKernelArg("itemCount1", 2, sizeof(unsigned), (void*)&nTransactions); 
    setKernelArg("itemCount1", 3, sizeof(unsigned), (void*)&nItems); 

    size_t szLocalWorkSize = NO_THREADS_BLOCK;
    size_t szGlobalWorkSize;
    if (nItems % 256 == 0) { 
        szGlobalWorkSize = nItems;
    } else {
        szGlobalWorkSize = 256 * ((nItems / 256) + 1);

    }
    
    runKernel("itemCount1", szLocalWorkSize, szGlobalWorkSize);
#endif
#if 0
    createKernel("itemCount2");
    setKernelArg("itemCount2", 0, sizeof(cl_mem), &dMarketBasket);
    setKernelArg("itemCount2", 1, sizeof(cl_mem), &dCount);
    setKernelArg("itemCount2", 2, sizeof(int), 0);
    setKernelArg("itemCount2", 3, sizeof(unsigned), (void*)&nTransactions);
    setKernelArg("itemCount2", 4, sizeof(unsigned), (void*)&nItems);

    size_t szLocalWorkSize = NO_THREADS_BLOCK;  
    size_t szGlobalWorkSize = nItems * NO_THREADS_BLOCK; 

    runKernel("itemCount2", szLocalWorkSize, szGlobalWorkSize); 
#endif
#if 1 
    unsigned* buff = (unsigned*)allocateHostMemory(sizeof(unsigned) * 256);
    for (unsigned i = 0; i < 256; ++i) {
        buff[i] = 255;
    }
    cl_mem dBuff = allocateDeviceMemory(buff, sizeof(unsigned) * 256,
                                               CL_MEM_READ_ONLY);
    createKernel("testbit");
    setKernelArg("testbit", 0, sizeof(cl_mem), &dBuff);

    size_t szLocalWorkSize = NO_THREADS_BLOCK;
    size_t szGlobalWorkSize = NO_THREADS_BLOCK;

    runKernel("testbit", szLocalWorkSize, szGlobalWorkSize);
#endif
    waitForEvent();
    copyFromDevice(dCount, count, nItems * sizeof(int));


    printGpuTime();
    verifyCount(count);
    cleanup();
}

void
gpuAssocBitmap(const char* file)
{
    fileReadBitmap(file);
    initExecution();
    int* count = (int*)allocateHostMemory(nItems * sizeof(int));
    for (unsigned i = 0; i < nItems; ++i) {
        count[i] = 0;
    }
    const int marketBasketSize = sizeof(unsigned) * nItems * nIntegers;
    cl_mem dMarketBasketBitmap = allocateDeviceMemory(marketBasketBitmap, 
                                                 marketBasketSize,
                                                CL_MEM_READ_ONLY);
    cl_mem dCount = allocateDeviceMemory(count, nItems * sizeof(int),
                                               CL_MEM_WRITE_ONLY);

    cl_mem dBitLookup = allocateDeviceMemory(bitLookup, sizeof(uchar) * 65536,
                                             CL_MEM_READ_ONLY);
    compileProgram(0, "kernel.cl");
    createKernel("itemCount3");
    setKernelArg("itemCount3", 0, sizeof(cl_mem), &dMarketBasketBitmap);
    setKernelArg("itemCount3", 1, sizeof(cl_mem), &dCount);
    setKernelArg("itemCount3", 2, sizeof(cl_mem), &dBitLookup);
    setKernelArg("itemCount3", 3, sizeof(int), 0);
    setKernelArg("itemCount3", 4, sizeof(unsigned), (void*)&nIntegers);
    setKernelArg("itemCount3", 5, sizeof(unsigned), (void*)&nItems);

    size_t szLocalWorkSize = NO_THREADS_BLOCK;
    size_t szGlobalWorkSize = nItems * NO_THREADS_BLOCK;

    runKernel("itemCount3", szLocalWorkSize, szGlobalWorkSize);
    waitForEvent();
    copyFromDevice(dCount, count, nItems * sizeof(int));
    printGpuTime();
    verifyBitmapCount(count);
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
           cout << "**********ERROR in item " << k <<  endl << endl;;
           cout << "Actual: "  << countI << "Result: " << count[k] << endl;
           return;
        } 
	}
    cout << "Verified" << endl << endl;
}

void
verifyBitmapCount(int* count)
{
   for (unsigned k = 0; k < nItems; ++k) {
	   unsigned countI = 0;
	   for (unsigned i = 0; i < nIntegers; ++i ) {
		   bitset<32> bTrans=  marketBasketBitmap[(k * nIntegers) + i];
		   countI += bTrans.count();
	   }
       if (countI != count[k]) {
           cout << "**********ERROR in item " << k <<  endl << endl;;
           cout << "Actual: "  << countI << "Result: " << count[k] << endl;
           return;
        }
    }
    cout << "Verified" << endl << endl;
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

