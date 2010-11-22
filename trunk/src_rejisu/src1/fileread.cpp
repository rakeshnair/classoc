#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<assoc.h>
#include<assert.h>
using namespace std;

bool* marketBasket;
unsigned nTranscations = 0;
unsigned nItems = 0;


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

    bool* marketBasket = (bool*)malloc(sizeof(bool) * nItems * nTranscations);
    
    unsigned tTrans = 0;
    while(!fp.eof()) {
       string line;
       getline(fp, line); 
       if (line[0] == '\0') break;
       stringstream s(line);
       unsigned item;
       while (!s.eof()) {
           s >> item;
           marketBasket[(item * nTranscations) + tTrans] = 1;
       }
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
