#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<assoc.h>
#include<assert.h>
#include"primes.h"
using namespace std;

bool* marketBasket;
unsigned gNTranscations = 0;
unsigned gNItems = 0;
unsigned long *gTransactionSet;
int gNoOfPrimes =  0;


void
fileRead(const char * xFileName)
{
    ifstream fp;
    fp.open(MARKET_BASKET_FILE, ios::in);
    if (!fp.is_open()) {
        cout <<  "Not Able to Open file " << MARKET_BASKET_FILE << endl;
        exit(0);
    }
    while(!fp.eof()) {
        string line;
        getline(fp, line);
        if (line[0] == '\0') break;
        stringstream s(line);
        unsigned item; 
        while (!s.eof()) {
            s >> item;
            if (item > gNItems) gNItems = item;
            //if (s.eof()) break;
        } 
        ++gNTranscations;
    }
    ++gNItems; // 0 is an item
    cout << "Market Basket File: "  << MARKET_BASKET_FILE << endl
         << "Transcations: " << gNTranscations << endl
         << "Items: " << gNItems << endl;

    fp.close();
    fp.open(MARKET_BASKET_FILE, ios::in); 

     marketBasket = (bool*)malloc(sizeof(bool) * gNItems * gNTranscations);
	 gTransactionSet = (unsigned long*)malloc(sizeof(long) * gNTranscations* NO_DIV_PRIME);
	 for(int i =0 ; i< gNTranscations* NO_DIV_PRIME ; ++i)
		 gTransactionSet[i]=1;
    
    unsigned tTrans = 0;
	int lPart =  gNItems/NO_DIV_PRIME + 1;
	// this has been done to cover up the extra Part
	// Only in the case of the last one does it taki the last prime Number
	gNoOfPrimes = lPart; 
    while(!fp.eof())
   	{
		string line;
		getline(fp, line); 
		if (line[0] == '\0') break;
		stringstream s(line);
		unsigned item;
		while (!s.eof())
		{
			s >> item;
			marketBasket[(item * gNTranscations) + tTrans] = 1;
			int lLocation  = item/lPart;
			unsigned long lTemp =   gTransactionSet[NO_DIV_PRIME *tTrans +lLocation];
			gTransactionSet[NO_DIV_PRIME *tTrans +lLocation] = gTransactionSet[ NO_DIV_PRIME * tTrans + lLocation] * primes[item% lPart];
			// This is done to check whethere anything goes out of Bounds
			if(lTemp >gTransactionSet[NO_DIV_PRIME *tTrans     +lLocation])
				cout<<"REJISUUUU\n";



		}
		// cout<< gTransactionSet[tTrans] << "Value"<<endl;
		++tTrans;  
    }

#if 0
    unsigned count = 0;
    cout << "test" << endl;
    for (unsigned i = 0; i < nTranscations; ++i ) {
        if (marketBasket[7115 * nTranscations + i] == 1) {
        //    cout << i + 1 << endl; 
            ++count;
        }
    }
    cout << count << endl;
#endif
        
    fp.close();
}
