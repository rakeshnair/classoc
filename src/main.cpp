#include<iostream>
#include<assoc.h>
#include<stdlib.h>
using namespace std;


int 
main(int argc, char* argv[])
{
    if (argc != 2) {
        cout << "Usage: ./assoc market_basket_file" << endl;
        return 1;
    }
    //test(); 
    gpuAssoc(argv[1]);
    return 0;
}

