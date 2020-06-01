#pragma once
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <stdlib.h>
#include "bignum.h"
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

// n*n
bignum sqr(const bignum &n) {
    mpz_t res;
    mpz_init(res);
    mpz_pow_ui(res, *n.valuePtr(), 2);

    bignum ret(res);
    mpz_clear(res);
    return ret;
}

bignum sqrt(const bignum &n) {
    mpz_t res;
    mpz_init(res);
    mpz_sqrt(res, *n.valuePtr());

    bignum ret(res);
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

bignum lcm(const bignum &a, const bignum &b) {
    return a*b / gcd(a,b);
}

bignum powersmall(const bignum &x, int n) {
    mpz_t res;
    mpz_init(res);
    mpz_pow_ui(res, *x.valuePtr(), n);

    bignum ret(res);
    mpz_clear(res);
    return ret;
}

bignum power(const bignum &x, const bignum &n) {
    if (n < 2000000000) return powersmall(x,int(n));
    if (n==0) return 1;
    bignum j = power(x, n/2);
    if (n%2 == 0) return j*j;
    else return j*j*x;
}

bignum powermod(const bignum &x, const bignum &n, const bignum &base) {
    mpz_t res;
    mpz_init(res);
    mpz_powm(res, *x.valuePtr(), *n.valuePtr(), *base.valuePtr());

    bignum ret(res);
    mpz_clear(res);
    return ret;
}


/***********************************************/
/**    Modular functions      */

pt extendedGcd(const bignum &a, const bignum &b) {
    if (b==0) return pt(1, 0);
    pt qr, st;

    qr = pt(a/b, a%b);
    st = extendedGcd(b, qr.Y);
    return pt(st.Y, st.X - qr.X * st.Y);
}


bignum inverse(const bignum &a, const bignum &base)
{
    pt xy = extendedGcd(a, base);
    bignum res = xy.X % base;
    if (res < 0) res += base;
    return res;
}

// using chinese remainder theorem to solve pair of modular equation
bignum crt2(const bignum &a1, const bignum &a2, const bignum &n1, const bignum &n2)
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

