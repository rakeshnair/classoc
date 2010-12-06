#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
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
	 int lTransactionLoc= bx* 256 + tx;
	 if(tx<gNItems)
		 dFreqItemCountSh[tx] = 0;
	 barrier(CLK_LOCAL_MEM_FENCE);
	 if(lTransactionLoc >=gNTranscations *NO_DIV_PRIME)
		 return;
	 unsigned long lTransaction = dTransactions[lTransactionLoc];
	 int lItemCount =  (bx%NO_DIV_PRIME) * gNoOfPrimes;
	 for(int j =0 ;j <gNoOfPrimes ; ++j)
	 {
		 int lCurrentItem = lItemCount + j;
		 if(lTransaction % gPrimes[j]==0)
		 {
			 if(dFreqItems[lCurrentItem] == false)
			 {
				 atom_add(dFreqItemCount +  lCurrentItem,1);
				 if(dFreqItemCount[ lCurrentItem] >= SUPPORT_COUNT)
					 dFreqItems[ lCurrentItem] = true;		 
			 }
		 }
	 }
	 barrier(CLK_LOCAL_MEM_FENCE);
	 /*
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
