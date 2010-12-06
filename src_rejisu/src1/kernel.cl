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
//	__global bool* const dFreqItems,
	__kernel void
mItemCount(__global const long * const dTransactions,
		__global int* const dFreqItems,
		__constant int* gPrimes,
		const unsigned gNTranscations,
		const unsigned gNItems,
		const unsigned gSupportCount,
		const unsigned gNoOfPrimes)
{
	 const size_t bx = get_group_id(0);
	 const size_t tx = get_local_id(0);
	 int lSet = bx* 256 + tx;
	 if(tx< gNItems)
	 {
		 //dFreqItems[lSet] = lSet;
		 //dFreqItems[lSet] = gPrimes[lSet%gNoOfPrimes ];
		  dFreqItems[tx] = tx;
//		 dFreqItems[tx] = gPrimes[tx % gNoOfPrimes];
	 }


}
