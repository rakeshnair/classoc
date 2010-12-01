#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<assoc.h>
#include<assert.h>
#include<clutil.h>
using namespace std;

bool* marketBasket;
unsigned nTransactions = 0;
unsigned nItems = 0;


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
