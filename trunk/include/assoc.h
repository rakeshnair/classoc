#ifndef ASSOC_H
#define ASSOC_H

#define MARKET_BASKET_FILE "../test/pumsb.dat"
#define NO_THREADS_BLOCK 256
#define SIZE  1048576

void fileRead();
void gpuAssoc();
void test();

extern bool*  marketBasket;


#endif
