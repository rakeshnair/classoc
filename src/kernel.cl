#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_amd_printf : enable

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

// kernel using bitmap
__kernel void
itemCount3(__global const uint* const dMarketBasketBitmap,
           __global int* const dCount, __constant uchar* dBitLookup,
           __local int* sCount, const unsigned nIntegers,
           const unsigned nItems)
{
    const size_t bx = get_group_id(0);
    const size_t tx = get_local_id(0);
    const unsigned item = bx;
    *sCount = 0;
    barrier(CLK_LOCAL_MEM_FENCE);
    const unsigned dataPerThread = nIntegers / 256 + 1;
    const unsigned startIndex = (item * nIntegers);
    const unsigned maxIndex = (bx + 1) * nIntegers;
    uint count = 0;
    for (unsigned i = 0; i < dataPerThread; ++i) { 
       const unsigned index = startIndex + (i * 256) + tx; 
       if (index >= maxIndex) break; 
       const uint val = (dMarketBasketBitmap[index]);
       const ushort upval = (val & 0xffff0000) >> 16;
       const ushort lowval = val & 0x0000ffff;
       count += (uint)dBitLookup[upval] + (uint)dBitLookup[lowval];
    }
    atom_add(sCount, count); 
    barrier(CLK_LOCAL_MEM_FENCE);
    if (tx == 0) {
        dCount[item] = *sCount;
    }
}


__kernel void 
countKTransaction(__global const uint* const dMarketBasketBitmap, 
           __global uchar* const dCount,
           __constant uchar* dBitLookup,
           __local int* sCount,
           const unsigned nIntegers, const unsigned supportValue,
           const unsigned workPerBlock) 
{
	const size_t bx = get_group_id(0);
	const size_t tx = get_local_id(0);
	const unsigned startItem = bx * workPerBlock;
	const unsigned dataPerThread = nIntegers / 256 + 1;
    if (tx == 0) {
	   *sCount = 0;
    }
	barrier(CLK_LOCAL_MEM_FENCE);

	for (unsigned k = 0; k < workPerBlock; ++k) {
		const unsigned startIndex = ((startItem + k) * nIntegers);
		const unsigned maxIndex = (startItem + k + 1) * nIntegers;
		uint count = 0;
		for (unsigned i = 0; i < dataPerThread; ++i) {
			const unsigned index = startIndex + (i * 256) + tx;
			if (index >= maxIndex) break;
			const uint val = (dMarketBasketBitmap[index]);
			const ushort upval = (val & 0xffff0000) >> 16;
			const ushort lowval = val & 0x0000ffff;
			count += (uint)dBitLookup[upval] + (uint)dBitLookup[lowval];
		}
        if (count)
		   atom_add(sCount, count);
		barrier(CLK_LOCAL_MEM_FENCE);
		if (tx == 0) {
			if (*sCount > supportValue) { 
				dCount[startItem + k] = 1;
			}
            *sCount = 0;
		}
        barrier(CLK_LOCAL_MEM_FENCE);
	}
}


__kernel void
countKItem(__global unsigned* dItemBitmap,
           __global uchar* dCount, __global unsigned* dNEntries,
           __local int* sCount, const unsigned nEntries)  
                                   
{
    const size_t bx = get_group_id(0);
    const size_t tx = get_local_id(0);
    const unsigned startIndex = bx;
    if (tx == 0) {
       *sCount = dCount[startIndex];
    }
    barrier(CLK_LOCAL_MEM_FENCE); 
    if (*sCount == 0) return;
    const unsigned entriesPerThread = (nEntries - bx) / 256 + 1;
    int count = 0;
    for (unsigned i = 0; i < entriesPerThread; ++i) {
         const unsigned index = startIndex + 1 + i * 256 + tx;
         if (index >= nEntries) break; 
         count += (uint)dCount[index]; 
    } 
    if (count)
       atom_add(sCount, count);
    barrier(CLK_LOCAL_MEM_FENCE);
    if (tx == 0) {
       atom_add(dNEntries, *sCount - 1);
    }
}

__kernel void
testbit(__global uint* dBuff)
{
    const size_t tx = get_local_id(0);
    ushort2  a = dBuff[tx];
    const uint upval = (a.x & 0xff00) >> 8;
    const uint lowval = a.y & 0x00ff;
    //printf("hello\n"); 
    printf("%d %u %u\n", tx, upval, lowval); 

}

