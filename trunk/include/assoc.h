#ifndef ASSOC_H
#define ASSOC_H

#define MARKET_BASKET_FILE "../test/chess.dat"
#define NO_THREADS_BLOCK 256
#define SIZE  1048576
#define NO_DIV_PRIME 4

void fileRead(const char* file);
void fileReadBitmap(const char* file);
void gpuAssoc(const char* file);
void gpuAssocBitmap(const char* file);
void test();
void verifyCount(int * count);
void verifyBitmapCount(int * count);

typedef unsigned char uchar;
extern uchar* bitLookup; 
extern unsigned* marketBasketBitmap;
extern unsigned nIntegers;
extern unsigned nTransactions;
extern bool*  marketBasket;
extern unsigned nItems;

#endif
