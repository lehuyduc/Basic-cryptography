#include <iostream>
#include <bignum.h>
#include <random>
#include <time.h>
using namespace std;

bignum factorial(int n)
{
    bignum res = 1;
    for (int i=2; i<=n; i++) res *= i;
    return res;
}

int main()
{
    int n;
    cout << "n = "; cin >> n;
    bignum res = factorial(n);
    cout << res << "\n";
    cout << res.getSize(10) << "\n";
    return 0;
}
