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
itemCount(__global const char* const dMarketBasket,
          __global int* dCount, const unsigned nTranscations,
          const unsigned nItems)
{
    const size_t bx = get_group_id(0);
    const size_t tx = get_local_id(0);
    const unsigned item = bx * 256 + tx;
    if (item >= nItems) return;
    const unsigned startIndex = item * nTranscations;
    int count = 0;
	for (unsigned i = 0; i <  nTranscations; ++i) {
        count += dMarketBasket[startIndex + i];
	}
    //printf("%d %d\n", item, count);
    dCount[item] = count;
}
