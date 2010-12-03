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
unsigned nTranscations = 0;
unsigned nItems = 0;
unsigned long *gTransactions;


void
fileRead()
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
            if (item > nItems) nItems = item;
            //if (s.eof()) break;
        } 
        ++nTranscations;
    }
    ++nItems; // 0 is an item
    cout << "Market Basket File: "  << MARKET_BASKET_FILE << endl
         << "Transcations: " << nTranscations << endl
         << "Items: " << nItems << endl;

    fp.close();
    fp.open(MARKET_BASKET_FILE, ios::in); 

     marketBasket = (bool*)malloc(sizeof(bool) * nItems * nTranscations);
	 gTransactions = (unsigned long*)malloc(sizeof(long) * nTranscations* NO_DIV_PRIME);
	 for(int i =0 ; i< nTranscations* NO_DIV_PRIME ; ++i)
		 gTransactions[i]=1;
    
    unsigned tTrans = 0;
	int lPart =  nItems/NO_DIV_PRIME;
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
			marketBasket[(item * nTranscations) + tTrans] = 1;
			int lLocation  = item/lPart;
			unsigned long lTemp =   gTransactions[NO_DIV_PRIME *tTrans +lLocation];
			gTransactions[NO_DIV_PRIME *tTrans +lLocation] = gTransactions[ NO_DIV_PRIME * tTrans + lLocation] * primes[item% lPart];
			if(lTemp >gTransactions[NO_DIV_PRIME *tTrans     +lLocation])
				cout<<"REJISUUUU\n";



		}
		// cout<< gTransactions[tTrans] << "Value"<<endl;
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
