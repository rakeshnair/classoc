#include<assoc.h>
#include<clutil.h>
#include"primes.h"
#include<string.h>


void
gpuAssoc( const char *xFileName)
{
	fileRead(MARKET_BASKET_FILE);
	initExecution();
	int lTransationSize =  gNTranscations * sizeof(long) * NO_DIV_PRIME;
	bool *gFreqItems = (bool*)malloc(sizeof(bool)* gNItems);
	int lFreqItemSize = sizeof(bool)* gNItems;
	memset(gFreqItems,0,lFreqItemSize);

	int *gFreqItemCount = (int*)malloc(sizeof(int)* gNItems);
	int lFreqItemCountSize =  sizeof(int)* gNItems;
	memset(gFreqItemCount,0, lFreqItemCountSize);


	cl_mem dTransactions  = allocateDeviceMemory(gTransactionSet,lTransationSize,
			 CL_MEM_READ_ONLY);
	cl_mem dFreqItems= allocateDeviceMemory(gFreqItems,lFreqItemSize,
			CL_MEM_WRITE_ONLY);
	cl_mem dFreqItemCount= allocateDeviceMemory(gFreqItemCount, lFreqItemCountSize,
			CL_MEM_WRITE_ONLY);

	cl_mem dPrimes =   allocateDeviceMemory(primes,gNoOfPrimes*sizeof(int),
			CL_MEM_READ_ONLY);

	compileProgram("primes.h", "kernel.cl");
	int gSupportCount =  SUPPORT_COUNT;
	createKernel("mItemCount");
	setKernelArg("mItemCount",0, sizeof(cl_mem),&dTransactions);
	setKernelArg("mItemCount",1, sizeof(cl_mem),&dFreqItems);
	setKernelArg("mItemCount",2, sizeof(cl_mem),&dFreqItemCount);
	setKernelArg("mItemCount",3,  lFreqItemCountSize , 0);
	setKernelArg("mItemCount",4, sizeof(cl_mem),&dPrimes);
	setKernelArg("mItemCount",5, sizeof(unsigned),(void*)&gNTranscations);
	setKernelArg("mItemCount",6, sizeof(unsigned),(void*)&gNItems);
	setKernelArg("mItemCount",7, sizeof(unsigned),(void*)&gSupportCount);
	setKernelArg("mItemCount",8, sizeof(unsigned),(void*)&gNoOfPrimes);
	size_t szLocalWorkSize = NO_THREADS_BLOCK;
	size_t szGlobalWorkSize = ((gNTranscations/NO_THREADS_BLOCK) + 1)*  NO_THREADS_BLOCK;
	// running kernel
	runKernel("mItemCount", szLocalWorkSize, szGlobalWorkSize);
	waitForEvent();
    printGpuTime();
	 copyFromDevice(dFreqItems, gFreqItems, gNItems* sizeof(bool));
	 copyFromDevice(dFreqItemCount, gFreqItemCount, gNItems* sizeof(int));
	 mPrintFreqItems(gFreqItems,gNItems);
	 mPrintFreqItemCount(gFreqItemCount,gNItems);



    // Assoc code goes ehre


    cleanup();
}
void mPrintFreqItemCount(int* xFreqItems,int xNItems)
{
	cout<<"The Frequent Items are "<< endl;
	for(int i =0; i<xNItems ;++i)
	{
			cout<<" "<< xFreqItems[i]  << " " ;
	}
	cout<<endl;
}
void mPrintFreqItems(bool * xFreqItems,int xNItems)
{
	cout<<"The Frequent Items are "<< endl;
	for(int i =0; i<xNItems ;++i)
	{
		if(xFreqItems[i] ==true)
			cout<<" "<< i  << " ";
	}
	cout<<endl;
}

