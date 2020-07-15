#include <iostream>
#include <bignum.h>
#include <random>
#include <time.h>
#include <rander.h>
using namespace std;

int k, seed, mode;
Rander rander;

bignum power(bignum x, bignum n);
bignum getKbitNum(Rander &rander, int kbits);
bignum getKdigitNum(Rander &rander, int kdigits);


void mode1()
{
    // generate random binary string, then convert to bignum
    bignum val = 1;
    while (val % 20 != 0) {
        val = getKbitNum(rander, k);
        cout << val.toString(2) << " " << val << "\n\n";
    }
}

void mode2()
{
    bignum maxNumber = power(10, k);
    bignum val = 1;
    while (val % 20 != 0) {
        val = rander.randomm(maxNumber);
        cout << val << "\n";
    }
}

void mode3()
{
    bignum val = 1;
    while (val % 20 != 0) {
        val = getKdigitNum(rander, k);
        cout << val << "\n";
    }
}

int main()
{
    srand(time(NULL));
    cout << "Input k and seed: ";cin >> k >> seed;
    cout << "Input mode (1=exact k bits, 2=max k bits, 3=exact k digits, else = 3): "; cin >> mode;

    rander.initSeed(seed);
    if (mode==1) mode1();
    else if (mode==2) mode2();
    else mode3();

    return 0;
}


/*********************************/
bignum power(bignum x,bignum n) {
    if (n==0) return 1;
    bignum j = power(x, n/2);
    if (n%2 == 0) return j*j;
    else return j*j*x;
}

bignum getKbitNum(Rander &rander, int kbits)
{
    string res = "1";
    for (int i=2; i<=kbits; i++)
        res += char(rander.randomm(2) + '0');
    return bignum(res.c_str(), 2);
}

bignum getKdigitNum(Rander &rander, int kdigits)
{
    string res = "";
    res += char(rander.randomm(9) + '1');
    for (int i=2; i<=kdigits; i++)
        res += char(rander.randomm(10) + '0');
    return bignum(res.c_str(), 10);
}
