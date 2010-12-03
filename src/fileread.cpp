#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<assoc.h>
#include<assert.h>
#include<clutil.h>
#include<bitset>
#include<string.h>
using namespace std;

bool* marketBasket = 0;;
unsigned nTransactions = 0;
unsigned nItems = 0;
unsigned nIntegers = 0;
unsigned* marketBasketBitmap = 0;
typedef unsigned char uchar;
uchar* bitLookup = 0;


void
fileRead(const char* file)
{
    ifstream fp;
    fp.open(file, ios::in);
    if (!fp.is_open()) {
        cout <<  "Not Able to Open file " << file << endl;
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
        ++nTransactions;
    }
    ++nItems; // 0 is an item
    cout << "Market Basket File: "  << file << endl
         << "Transcations: " << nTransactions << endl
         << "Items: " << nItems << endl;

    fp.close();
    fp.open(MARKET_BASKET_FILE, ios::in); 

    marketBasket = (bool*)allocateHostMemory(sizeof(bool) * nItems * nTransactions);
    unsigned tTrans = 0;
    while(!fp.eof()) {
       string line;
       getline(fp, line); 
       if (line[0] == '\0') break;
       stringstream s(line);
       unsigned item;
       while (!s.eof()) {
           s >> item;
           marketBasket[(item * nTransactions) + tTrans] = 1;
       }
       ++tTrans;  
    }
    fp.close();
}


void
fileReadBitmap(const char* file)
{
    ifstream fp;
    fp.open(file, ios::in);
    if (!fp.is_open()) {
        cout <<  "Not Able to Open file " << file << endl;
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
        ++nTransactions;
    }
    ++nItems; // 0 is an item
    cout << "Market Basket File: "  << file << endl
         << "Transcations: " << nTransactions << endl
         << "Items: " << nItems << endl;

    fp.close();
    fp.open(MARKET_BASKET_FILE, ios::in);
    if (nTransactions % 32) {
        nIntegers = (nTransactions / 32) + 1;
    } else {
        nIntegers = nTransactions / 32;
    }
    
    marketBasketBitmap = (unsigned*)allocateHostMemory(sizeof(int) * nIntegers *
                                                         nItems);
    memset(marketBasketBitmap, 0, sizeof(int) * nIntegers * nItems);
    unsigned maxitem = 0;
    unsigned tTrans = 0;
    while(!fp.eof()) {
       string line;
       getline(fp, line);
       if (line[0] == '\0') break;
       stringstream s(line);
       unsigned titem = 0;
       unsigned item;
       while (!s.eof()) {
           s >> item;
           bitset<32> bTrans(marketBasketBitmap[(item * nIntegers) + tTrans / 32]); 
           bTrans.set(tTrans % 32, 1);
           marketBasketBitmap[(item * nIntegers) + tTrans / 32] = bTrans.to_ulong();
           ++titem;
       }
       if (maxitem < titem) {
          maxitem = titem;
       }
       ++tTrans;
    }
    cout << "Max Items in a transaction: " << maxitem <<  endl;
#if 0
     unsigned countI = 0;
		for (unsigned i = 0; i < nIntegers; ++i ) {
			bitset<32> bTrans=  marketBasketBitmap[(74 * nIntegers) + i]; 
   		    countI += bTrans.count();
		}
        cout << "Bitmap" << countI << endl;

#endif
    bitLookup = (uchar*)allocateHostMemory(sizeof(uchar) * 65536);
    for (unsigned i = 0; i < 65536; ++i) {
        bitset<16> bits(i);
        bitLookup[i] = (uchar)bits.count();
    }
    fp.close();
}

    

