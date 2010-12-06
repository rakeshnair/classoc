#include<assoc.h>
#include<clutil.h>
#include"primes.h"


void
gpuAssoc( const char *xFileName)
{
	fileRead(MARKET_BASKET_FILE);
	initExecution();
	int lTransationSize =  gNTranscations * sizeof(long) * NO_DIV_PRIME;
	//bool *gFreqItems = (bool*)malloc(sizeof(bool)* gNItems);
	int *gFreqItems = (int*)malloc(sizeof(int)* gNItems);
	//int lFreqItemSize = sizeof(bool)* gNItems;
	int lFreqItemSize = sizeof(int)* gNItems;
	memset(gFreqItems,0,lFreqItemSize);
	cl_mem dTransactions  = allocateDeviceMemory(gTransactionSet,lTransationSize,
			 CL_MEM_READ_ONLY);
	cl_mem dFreqItems= allocateDeviceMemory(gFreqItems,lFreqItemSize,
			CL_MEM_WRITE_ONLY);
	cl_mem dPrimes =   allocateDeviceMemory(primes,gNoOfPrimes*sizeof(int),
			CL_MEM_READ_ONLY);

	compileProgram(0, "kernel.cl");
	int gSupportCount =  SUPPORT_COUNT;
	createKernel("mItemCount");
	setKernelArg("mItemCount",0, sizeof(cl_mem),&dTransactions);
	setKernelArg("mItemCount",1, sizeof(cl_mem),&dFreqItems);
	setKernelArg("mItemCount",2, sizeof(cl_mem),&dPrimes);
	setKernelArg("mItemCount",3, sizeof(unsigned),(void*)&gNTranscations);
	setKernelArg("mItemCount",4, sizeof(unsigned),(void*)&gNItems);
	setKernelArg("mItemCount",5, sizeof(unsigned),(void*)&gSupportCount);
	setKernelArg("mItemCount",6, sizeof(unsigned),(void*)&gNoOfPrimes);
	size_t szLocalWorkSize = NO_THREADS_BLOCK;
	size_t szGlobalWorkSize = SIZE;
	// running kernel
	runKernel("mItemCount", szLocalWorkSize, szGlobalWorkSize);
	waitForEvent();
	 copyFromDevice(dFreqItems, gFreqItems, gNItems* sizeof(int ));
	 mPrintFreqItems(gFreqItems,gNItems);



    // Assoc code goes ehre


    printGpuTime();
    cleanup();
}

//void mPrintFreqItems(bool * xFreqItems,int xNItems)
void mPrintFreqItems(int * xFreqItems,int xNItems)
{
	cout<<"The Frequent Items are "<< endl;
	for(int i =0; i<xNItems ;++i)
	{
//		if(xFreqItems[i] ==true)
			cout<<" "<< xFreqItems[i]  << " ";
	}
	cout<<endl;
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
