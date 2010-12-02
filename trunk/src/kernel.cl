#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable

__kernel void
vecAdd(__global const int* const a,
       __global const int* const b,
       __global int* const c)
{
     const size_t bx = get_group_id(0);
     const size_t tx = get_local_id(0);
     const unsigned index = bx * 256 + tx;
     c[index] = a[index] + b[index]; 
}


// Naive Kernel with each thread operates counts 1 item 
__kernel void
itemCount1(__global const char* const dMarketBasket,
          __global int* dCount, const unsigned nTransactions,
          const unsigned nItems)
{
    const size_t bx = get_group_id(0);
    const size_t tx = get_local_id(0);
    const unsigned item = bx * 256 + tx;
    if (item >= nItems) return;
    const unsigned startIndex = item * nTransactions;
    int count = 0;
	for (unsigned i = 0; i <  nTransactions; ++i) {
        count += dMarketBasket[startIndex + i];
	}
    //printf("%d %d\n", item, count);
    dCount[item] = count;
}


__kernel void
itemCount2(__global const char* const dMarketBasket,
          __global int* dCount, __local int* sCount,
           const unsigned nTransactions,
           const unsigned nItems)
{
    const size_t bx = get_group_id(0);
    const size_t tx = get_local_id(0);
    const unsigned item = bx;
    const unsigned transPerThread = nTransactions / 256 + 1;
    //const unsigned startIndex = (item * nTransactions) + (tx * transPerThread); 
    const unsigned startIndex = (item * nTransactions) ; 
    const unsigned maxIndex = (bx + 1) * nTransactions;
    *sCount = 0;
    barrier(CLK_LOCAL_MEM_FENCE);

    int count = 0; 
    for (unsigned i = 0; i < transPerThread; ++i) {
        //if (startIndex + i >= maxIndex) break;
        //count += dMarketBasket[startIndex + i]; 
        // coalesced access
        const unsigned index = startIndex + (i * 256) + tx;
        if (index >=  maxIndex) break; 
        count += dMarketBasket[index];
    }
    atom_add(sCount, count);        
    barrier(CLK_LOCAL_MEM_FENCE);  
    if (tx == 0) {
        dCount[item] = *sCount;
    }
}
