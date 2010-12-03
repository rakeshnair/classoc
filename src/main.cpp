#include<iostream>
#include<assoc.h>
#include<stdlib.h>
#include<bitset>
using namespace std;

void testint()
{
    bitset<32> mybits;
    for (unsigned i = 0; i < 32; ++i)
    {
        if (i % 2) {
            mybits.set(i , 1);
        } else {
            mybits.set(i , 1);
        }
    }
    cout << mybits.to_ulong() << endl; 
    cout << sizeof(short) << endl;
}


int 
main(int argc, char* argv[])
{
    if (argc != 2) {
        cout << "Usage: ./assoc market_basket_file" << endl;
        return 1;
    }
    //test(); 
    //gpuAssoc(argv[1]);
    gpuAssocBitmap(argv[1]);
    return 0;
}

