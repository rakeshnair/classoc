#include<stdio.h>
#include<stdlib.h>
#include<math.h>
int main(int argc,char * argv[])
{
	if(argc <2)
	{
		printf(" Add the Number of Primes as argument \n");
		return 0;
	}
	int lNofPrimes = 1;
	FILE *fp = fopen("prime_number","w");
	float lNumber =2;
	fprintf(fp,"2,");
	while(1)
	{
		++lNumber;
		if(lNofPrimes==atoi(argv[1]))
			break;
		int lIsPrime = 1;
		int lIter =0;
		for(lIter = 2; lIter <= (int)sqrtf(lNumber); ++lIter)
		{
			if(((int)lNumber)%lIter ==0)
			{
				lIsPrime = 0 ;
				break;
			}

		}
		if(lIsPrime)
		{
			lNofPrimes++;
			fprintf(fp,"%d,",(int)lNumber);
		}


	}
	fclose(fp);

	printf("Check File prime_number for  output\n");



}
