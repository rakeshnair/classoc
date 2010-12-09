#include<assoc.h>
#include<clutil.h>
#include<assert.h>
#include<bitset>
#include<string.h>
const unsigned supportValue = 3000;
unsigned* itemBitmap = 0;
unsigned nItemIntegers = 0;
unsigned nEntries = 0;
unsigned itemset = 1;
const unsigned maxBlocks = 16384;

void
gpuAssoc(const char* file)
{
    fileRead(file);
    initExecution();
    int* count = (int*)malloc(nItems * sizeof(int));
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
#if 1
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
#if 0 
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
    compileProgram(0, "kernel.cl");


    nEntries = nItems;
    cl_mem dBitLookup = allocateDeviceMemory(bitLookup, sizeof(uchar) * 65536,
                                                CL_MEM_READ_ONLY);

    bool* count = 0;
    cl_mem dMarketBasketBitmap;
    cl_mem dCount; 
    createKernel("countKTransaction");
    createItemBitmap();
#if 0
    cl_mem dItemBitmap;
    unsigned zero = 0;
    cl_mem dNEntries = allocateDeviceMemory(&zero, sizeof(unsigned),
                                           CL_MEM_READ_WRITE);

    createKernel("countKItem");
#endif

	while(1)
	{
        cout << "ITEMSET " << itemset << " of size " << nEntries << endl;
		count = (bool*)malloc(nEntries * sizeof(bool));  
		memset(count, 0, nEntries * sizeof(bool));
        const int marketBasketSize = sizeof(unsigned) * nEntries * nIntegers;
        dMarketBasketBitmap = allocateDeviceMemory(marketBasketBitmap,
                                                          marketBasketSize,
                                                          CL_MEM_READ_ONLY, false);
        dCount = allocateDeviceMemory(count, nEntries * sizeof(bool),
                                                     CL_MEM_WRITE_ONLY, false);

        unsigned workPerBlock = 1;  
        size_t szLocalWorkSize = NO_THREADS_BLOCK;
        if (nEntries > maxBlocks){
           workPerBlock = nEntries / maxBlocks + 1;
        }
        
		size_t szGlobalWorkSize = (nEntries / workPerBlock) * NO_THREADS_BLOCK; 
		setKernelArg("countKTransaction", 0, sizeof(cl_mem), &dMarketBasketBitmap);
		setKernelArg("countKTransaction", 1, sizeof(cl_mem), &dCount);
		setKernelArg("countKTransaction", 2, sizeof(cl_mem), &dBitLookup);
		setKernelArg("countKTransaction", 3, sizeof(int), 0);
		setKernelArg("countKTransaction", 4, sizeof(unsigned), (void*)&nIntegers);
		setKernelArg("countKTransaction", 5, sizeof(unsigned), (void*)&supportValue);
		setKernelArg("countKTransaction", 6, sizeof(unsigned), (void*)&workPerBlock);
        cout << "Running kernel"<< endl;
		runKernel("countKTransaction", szLocalWorkSize, szGlobalWorkSize);
		waitForEvent(); 
        copyFromDevice(dCount, count, nEntries * sizeof(bool));

#if 0
        dItemBitmap = allocateDeviceMemory(0, nEntries * sizeof(unsigned), CL_MEM_READ_ONLY);

        setKernelArg("countKItem", 0, sizeof(cl_mem), &dItemBitmap);
        setKernelArg("countKItem", 1, sizeof(cl_mem), &dCount);
        setKernelArg("countKItem", 2, sizeof(cl_mem), &dNEntries);
		setKernelArg("countKItem", 3, sizeof(int), 0);
        setKernelArg("countKItem", 4, sizeof(unsigned), (void*)&nEntries);

		runKernel("countKItem", szLocalWorkSize, szGlobalWorkSize);
         
        const unsigned oldEntries = nEntries; 
		copyFromDevice(dNEntries, &nEntries, sizeof(unsigned));     

#endif
        if (!generateKItemSet(count)) {
            break; 
        }
        free(count);
        freeDeviceBuffer(dMarketBasketBitmap);
        freeDeviceBuffer(dCount);
        ++itemset;
	}
    printGpuTime();
    free(count);
    free(itemBitmap);
    free(marketBasketBitmap);
    cleanup();
}

void
createItemBitmap()
{
    nItemIntegers = nEntries / 32 + 1;
    itemBitmap = (unsigned*)malloc(sizeof(unsigned) * 
                              nEntries * nItemIntegers);
    memset(itemBitmap, 0, sizeof(unsigned) * nEntries * nItemIntegers);
    for (unsigned i = 0; i < nEntries; ++i) {
        bitset<32> bits(0);
        bits.set(31 - (i % 32), 1);
        itemBitmap[i * nItemIntegers + i / 32] = bits.to_ulong();
    }
}

bool
generateKItemSet(const bool* count)
{
    cout << "Candidate generation" << endl;
    unsigned cEntries = 0;
    for (unsigned i = 0; i < nEntries; ++i) {
        if (count[i] == 0) continue;
        for (unsigned j = i + 1; j < nEntries; ++j) {
            if(count[j] == 0) continue;
            if (itemset == 1) { ++cEntries; continue;}
            unsigned diff = 0;
            for (unsigned k = 0; k < nItemIntegers; ++k) {
                const unsigned iItem = itemBitmap[i * nItemIntegers + k];
                const unsigned jItem = itemBitmap[j * nItemIntegers + k];
                unsigned kDiff = iItem ^ jItem;
                while (kDiff)
                {
                   ++diff;
                   kDiff = kDiff & (kDiff - 1); 
                }
                if (diff != 2) break; 
            }
            if (diff != 2) continue;
            ++cEntries; 
        }
    }
    if (cEntries == 0) return false;


    unsigned* iBitmap = (unsigned*)malloc(sizeof(unsigned) *
                              cEntries * nItemIntegers);
    unsigned* tBitmap = (unsigned*)malloc(sizeof(unsigned) *
                               nIntegers * cEntries);

    unsigned index = 0;
	for (unsigned i = 0; i < nEntries; ++i) {
		if (count[i] == 0) continue;
		for (unsigned j = i + 1; j < nEntries; ++j) {
			if(count[j] == 0) continue;
			unsigned diff = 0;

			if (itemset > 1) { 
				for (unsigned k = 0; k < nItemIntegers; ++k) {
					const unsigned iItem = itemBitmap[i * nItemIntegers + k];
					const unsigned jItem = itemBitmap[j * nItemIntegers + k];
					unsigned kDiff = iItem ^ jItem;
					while (kDiff)
					{
						++diff;
						kDiff = kDiff & (kDiff - 1);
					}
					if (diff != 2) break;
				}
                if (diff != 2) continue;
			}
			for (unsigned p = 0; p < nItemIntegers; ++p) {
				iBitmap[index * nItemIntegers + p] =  
					itemBitmap[i * nItemIntegers + p] |
					itemBitmap[j * nItemIntegers + p];   
			}

			for (unsigned p = 0; p < nIntegers; ++p) {
				tBitmap[index * nIntegers + p] =
					marketBasketBitmap[i * nIntegers + p] &
					marketBasketBitmap[j * nIntegers + p]; 
			}
			++index;
		}
	}

    assert(index == cEntries);
	


    free(marketBasketBitmap);
    free(itemBitmap);
    marketBasketBitmap = tBitmap;
    itemBitmap = iBitmap;  
    nEntries = cEntries; 
    for (unsigned i = 0; i < nEntries; ++i) {
		unsigned count = 0;
		for (unsigned j = 0; j < nItemIntegers; ++j) {
			bitset<32> bits(itemBitmap[i * nItemIntegers + j]);
			count += bits.count();
		}
		assert(count == (itemset + 1));
	}
    
    cout << "Candidate generation done" << endl;
    return true;
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
verifyItemSet(bool* count, const unsigned e)
{
    unsigned cEntries = 0;
    for (unsigned k = 0; k < e; ++k) {
        if (count[k] == 0) continue;  
        for (unsigned i = k + 1; i < e; ++i) {
           cEntries += count[i];
        }
    }
    if (cEntries != nEntries) {
       cout << "*********ERROR" << " Actual: " << cEntries 
                                << " Result: " << nEntries  
                                << endl;
       return;
    } 
    cout << "Verified"<< endl << endl;;    
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

