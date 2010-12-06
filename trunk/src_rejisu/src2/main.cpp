#include<iostream>
#include<assoc.h>
#include<stdlib.h>




int 
main()
{
    //test(); 
    gpuAssoc(MARKET_BASKET_FILE);
    free(marketBasket);
}

