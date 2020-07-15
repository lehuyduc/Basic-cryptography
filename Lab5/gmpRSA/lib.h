#pragma once
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <stdlib.h>
using std::string;
using std::cin;
using std::cout;
using std::pair;
using std::swap;

#define X first
#define Y second

typedef pair<bignum, bignum> pt;

const int maxn = 10000000;

/***********************************************/
/**             Basic functions                */

bignum sqrt(bignum n) {
    mpz_t res;
    mpz_init(res);
    mpz_sqrt(res, *n.valuePtr());
    bignum ret = bignum(res);
    mpz_clear(res);
    return ret;
}

bignum gcd(bignum a,bignum b) {
    while (b > 0) {
        a %= b;
        swap(a,b);
    }
    return a;
}

bignum lcm(bignum a, bignum b) {
    return a*b / gcd(a,b);
}

bignum power(bignum x,bignum n) {
    if (n==0) return 1;
    bignum j = power(x, n/2);
    if (n%2 == 0) return j*j;
    else return j*j*x;
}

bignum powermod(bignum x, bignum n, bignum base) {
    if (n==0) return 1;
    bignum j = powermod(x, n/2, base);
    if (n%2 == 0) return j*j%base;
    else return ((j*j % base) * x) % base;
}


/***********************************************/
/**    Prime number and modular functions      */

bignum nextPrime(bignum n) {
    mpz_t res;
    mpz_init(res);
    mpz_nextprime(res, *n.valuePtr());

    bignum ret = bignum(res);
    mpz_clear(res);
    return ret;
}

pt extendedGcd(bignum a, bignum b) {
    if (b==0) return pt(1, 0);
    pt qr, st;

    qr = pt(a/b, a%b);
    st = extendedGcd(b, qr.Y);
    return pt(st.Y, st.X - qr.X * st.Y);
}


bignum inverse(bignum a, bignum base)
{
    pt xy = extendedGcd(a, base);
    bignum res = xy.X % base;
    if (res < 0) res += base;
    return res;
}

bignum crt2(bignum a1, bignum a2, bignum n1, bignum n2)
{
    if (a1==0 && a2==0) return 0;
    if (n1==0 && n2==0) return -1; // infinite solution
    if (n1==0) return a2;
    if (n2==0) return a1;

    bignum d = gcd(n1, n2);
    pt xy = extendedGcd(n1, n2);
    bignum res = a1 + (xy.X * (a2 - a1) * n1) / d;
    bignum tmp = lcm(n1, n2);
    res %= tmp;
    if (res < 0) res += tmp;
    return res;
}
