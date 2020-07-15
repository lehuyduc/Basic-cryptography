#include <iostream>

#include "lib.h"
using namespace std;

extern const int maxn;

extern bool initedPrime;
extern bool isPrime[maxn+2];
extern vector<long long> primes;

int main()
{
    init();
    //testPrime(10000);
    //testPhi(10000);
    ll a = inverse(3458, 4864);
    cout << a << " " << (((3458*a%4864)+4864) % 4864) << "\n";

    ll b = inverse(1013, 4864);
    cout << b  << " " << (1013 * b % 4864) << "\n";
    return 0;
}
