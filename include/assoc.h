#ifndef ASSOC_H
#define ASSOC_H

#define MARKET_BASKET_FILE "../test/chess.dat"
#define NO_THREADS_BLOCK 256
#define SIZE  1048576

void fileRead(const char* file);
void gpuAssoc(const char* file);
void test();
void
verifyCount(int * count);


extern unsigned nTransactions;
extern bool*  marketBasket;
extern unsigned nItems;

#endif
