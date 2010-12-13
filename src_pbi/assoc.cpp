#include<assoc.h>
#include<clutil.h>
#include<assert.h>
#include<bitset>
#include<string.h>
#include<Timer.h>
#include "Trie.h"
const unsigned supportValue = 2000;
unsigned nEntries = 0;
unsigned itemset = 1;
const unsigned maxBlocks = 16384;
Trie* trie  = 0;
bool* count = 0;
vector<unsigned> newBitmap;

void
gpuAssocBitmap(const char* file)
{
    fileReadBitmap(file);
    initExecution();
    compileProgram(0, "kernel.cl");
     
    nEntries = nItems;
    trie = new Trie(nItems);
    cl_mem dBitLookup = allocateDeviceMemory(bitLookup, sizeof(uchar) * 65536,
                                                CL_MEM_READ_ONLY);

    count = (bool*)malloc(nEntries * sizeof(bool));
    
    const int marketBasketSize = sizeof(unsigned) * nEntries * nIntegers;
    cl_mem dMarketBasketBitmap =  allocateDeviceMemory(marketBasketBitmap,
                                                       marketBasketSize,
                                                       CL_MEM_READ_ONLY, false);
    cl_mem dCount = allocateDeviceMemory(count, nEntries * sizeof(bool),
                                                     CL_MEM_WRITE_ONLY, false);
    createKernel("countKTransaction");
    unsigned nOldEntries = nEntries;
    Timer t;
    t.Restart();
	while(1)
	{
        cout << "ITEMSET " << itemset << " of size " << nEntries << endl;

        const int marketBasketSize = sizeof(unsigned) * nEntries * nIntegers;   
        if (nOldEntries < nEntries) {
            free(count);
            count = (bool*)malloc(nEntries * sizeof(bool));
            memset(count, 0 ,nEntries * sizeof(bool));
            free(marketBasketBitmap);
            marketBasketBitmap = (unsigned*)malloc(sizeof(unsigned) * nIntegers *
                                                         nEntries);
            assert(newBitmap.size() == nIntegers * nEntries);
            copy(newBitmap.begin(), newBitmap.end(), marketBasketBitmap);  
            for (unsigned i = 0; i < newBitmap.size(); ++i) {
                assert(newBitmap[i] == marketBasketBitmap[i]);
            }
            freeDeviceBuffer(dMarketBasketBitmap);
            dMarketBasketBitmap = allocateDeviceMemory(marketBasketBitmap, marketBasketSize,
                                                CL_MEM_READ_ONLY, false);
             
            freeDeviceBuffer(dCount);
            dCount = allocateDeviceMemory(count, nEntries * sizeof(bool),
                                               CL_MEM_WRITE_ONLY, false); 
            nOldEntries = nEntries;
        } else {
            memset(count, 0 ,nEntries * sizeof(bool));
            copyToDevice(dCount, count,  nEntries * sizeof(bool));
            copyToDevice(dMarketBasketBitmap, marketBasketBitmap, marketBasketSize); 
        } 

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

        cout << "Copy From device" << endl;
        if (!generateKItemSet(count)) {
            break; 
        }
        
        cout << "item set generation done" << endl;
        ++itemset;
	}
    cout << "Total time " << t.GetTime() << " seconds" <<  endl;
    printGpuTime();
    freeDeviceBuffer(dMarketBasketBitmap);
    freeDeviceBuffer(dCount);
 
    free(count);
    free(marketBasketBitmap);
    delete trie;
    cleanup();
}

bool
generateKItemSet(const bool* count)
{
    newBitmap.clear();
    unsigned index = 0;
    if (itemset == 1) {
       for (unsigned i = 0; i < nItems; ++i) {
           if (count[i] == 0) continue; 
		   for (unsigned j = i + 1; j < nItems; ++j) {
			   if(count[j] == 0) continue;
			   vector<unsigned> elem;
			   elem.push_back(i);
			   elem.push_back(j);
			   trie->add(elem, index);
               performAnd(i, j);
			   ++index;
		   }
       }
       nEntries = index;
	   return true;   
    }
    cout << "start gen " << endl; 
	vector<vector<unsigned > > kItemSet;
    for (unsigned i = 0; i < nItems; ++i) {
        const TrieNode* node = trie->getRootNode(i); 
        if (!node) continue;   
        const NodeMap& map = node->data;
        vector<unsigned> prefix;
        prefix.push_back(i);
        traverse(map, prefix, kItemSet);
        
    }  
    
    cout << "end gen " << endl; 
    if (kItemSet.size() == 0) return false;
    assert(newBitmap.size() == kItemSet.size() * nIntegers );

    for (unsigned i = 0; i < kItemSet.size(); ++i) {
        assert( kItemSet[i].size() == itemset + 1); 
        trie->add( kItemSet[i], i);
    }
     
    nEntries =  kItemSet.size();
    return true; 
}

void
traverse(const NodeMap& nMap, vector<unsigned>& prefix,  vector<vector<unsigned > >&  kItemSet)
{
    NodeMap::const_iterator itr = nMap.begin();
    for (; itr != nMap.end(); ++itr) {
         TrieNode* node = (*itr).second.child;
         if (node) {
            prefix.push_back((*itr).first);
            traverse(node->data, prefix, kItemSet); 
            prefix.pop_back();  
         }
    }
      
    assert(prefix.size()); 
    if (prefix.size() != itemset - 1) return;
    itr = nMap.begin(); 
    for ( ; itr != nMap.end(); ++itr) {
        if (count[(*itr).second.index] == 0) continue;
        NodeMap::const_iterator itr1 = (++itr); --itr; 
        for (; itr1 != nMap.end(); ++itr1) {
             if (count[(*itr1).second.index] == 0) continue;
             prefix.push_back((*itr).first);
             prefix.push_back((*itr1).first);         
             if (trie->checkSupport(prefix, count));
             {
                 kItemSet.push_back(prefix);
                 performAnd((*itr).second.index, (*itr1).second.index);
             }
             prefix.pop_back(); 
             prefix.pop_back(); 
        }
    }
} 

void performAnd(const unsigned i, const unsigned j)
{
      for (unsigned p = 0; p < nIntegers; ++p) {
          newBitmap.push_back( marketBasketBitmap[i * nIntegers + p] &
                               marketBasketBitmap[j * nIntegers + p]);
      }
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

