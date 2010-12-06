#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
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
		__global bool* const dFreqItems,
		__global int* const dFreqItemCount,
		__local int* const dFreqItemCountSh,
		__constant int* gPrimes,
		const unsigned gNTranscations,
		const unsigned gNItems,
		const unsigned gSupportCount,
		const unsigned gNoOfPrimes)
{
	 const size_t bx = get_group_id(0);
	 const size_t tx = get_local_id(0);
	 int lTransactionIndex = bx* 256 + tx;
	 if(tx<gNItems)
		 dFreqItemCountSh[tx] = 0;
	 barrier(CLK_LOCAL_MEM_FENCE);
	 if(lTransactionIndex>= gNTranscations)
		 return;
	 int lItemCount =0;
	 for(int i= 0 ;i<NO_DIV_PRIME ;++i)
	 {
		 int lTransactionLoc = lTransactionIndex * NO_DIV_PRIME + i;
		 unsigned long lTransaction = dTransactions[lTransactionLoc];
		 for(int j =0 ;j <gNoOfPrimes ; ++j)
		 {
			 if(lTransaction % gPrimes[j]==0)
			 {
				 if(dFreqItems[lItemCount] == false)
				 {
					 atom_add(dFreqItemCountSh + lItemCount,1);
					 if(dFreqItemCountSh[lItemCount] >= SUPPORT_COUNT)
						 dFreqItems[lItemCount] = true;		 
				 }
			 }
			 lItemCount++;
		 }
	 }
	 barrier(CLK_LOCAL_MEM_FENCE);
	 if(tx<gNItems)
	 {
		 if(dFreqItems[tx] == false)
		 {
			 int lVal = dFreqItemCountSh[tx];
			 atom_add(dFreqItemCount + tx,lVal);
			 if(dFreqItemCount[tx] >= SUPPORT_COUNT)
				 dFreqItems[tx] = true;		 
		 }

	 }

}
