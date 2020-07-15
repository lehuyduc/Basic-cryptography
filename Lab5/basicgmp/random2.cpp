#include <iostream>
#include <bignum.h>
#include <random>
#include <time.h>
#include <rander.h>
using namespace std;

int k;
unsigned int seed;

int main()
{
    cout << "Input k and seed: "; cin >> k >> seed;
    Rander rander(seed);

    bignum val = 1;
    while (val % 20 != 0) {
        val = rander.randomb(k);
        cout << val << "\n";
    }

    return 0;
}


