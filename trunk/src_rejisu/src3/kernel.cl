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
	 int lNoOfTransactions = 0;
	 if(tx<gNItems)
		 dFreqItemCountSh[tx] = 0;
	 barrier(CLK_LOCAL_MEM_FENCE);
	 int  lTransactionIndex;
	 long lCurrPrime = gPrimes[bx%gNoOfPrimes];
	 int lNoOfTran =  gNTranscations/256 +1;
	 int lIndexofPrime = bx/gNoOfPrimes;
	 if(bx<gNItems)
		 dFreqItemCount[bx] =  lNoOfTran;
	 for(int i= 0 ;lNoOfTran ;++i)
	 {
		 int  lTransactionIndex =   i* 256 + tx;
		 if(lTransactionIndex > gNTranscations)
		 {
			 break;
		 }
		 int lTransactionLoc = lTransactionIndex * NO_DIV_PRIME + lIndexofPrime;
		 unsigned long lTransaction = dTransactions[lTransactionLoc];
		 if(lTransaction % lCurrPrime==0)
		 {
			 lNoOfTransactions++;

		 }
	 }
	  barrier(CLK_LOCAL_MEM_FENCE);
	  atom_add(dFreqItemCountSh + bx,lNoOfTransactions);
	  barrier(CLK_LOCAL_MEM_FENCE);
	  if(tx==0)
		  if(dFreqItemCountSh[bx] >=SUPPORT_COUNT)
			  dFreqItems[bx] = true;
	  dFreqItemCount[bx] = dFreqItemCountSh[bx];

/*
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
	 */

}
