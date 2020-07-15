#include <iostream>

#include "lib.h"
using namespace std;

typedef unsigned char byte;


pt getPrivatePublic(ll p, ll q)
{
    ll n, phin, e, d, k;
    n = p*q;
    phin = (p-1)*(q-1);
    e = 2;
    while (gcd(e, phin)!=1) e++;
    d = (extendedGcd(e, phin).X + lcm(e, phin)) % lcm(e, phin);
    return pt(d, e);
}

ll rsaEncrypt(ll mess, ll n, ll e)
{
    return powermod(mess, e, n);
}

ll rsaDecrypt(ll cipher, ll n, ll d)
{
    return powermod(cipher, d, n);
}

//----
ll crt2(ll a1, ll a2, ll n1, ll n2)
{
    ll d = gcd(n1, n2);
    pt xy = extendedGcd(n1, n2);
    ll res = a1 + xy.X * (a2 - a1) * n1 / d;
    if (res < 0) res += lcm(n1,n2);
    return res;
}

ll rsaDecryptCRT(ll c, ll n, ll d, ll p, ll q)
{
    ll dp = d % (p-1), dq = d % (q-1);
    ll mp = powermod(c % p, dp, p);
    ll mq = powermod(c % q, dq, q);
    return crt2(mp, mq, p, q);
}


int main()
{
    ll m, c, p, q, n , d, e;
    pt tmp;

    p = 163;
    q = 179;
    n = p*q;
    tmp = getPrivatePublic(p, q);
    d = tmp.X;
    e = tmp.Y;

    m = 151;
    c = rsaEncrypt(m, n, e);
    cout << "c = " << rsaDecrypt(c, n, d) << "\n";
    cout << "c = " << rsaDecryptCRT(c, n, d, p, q) << "\n";
    return 0;
}
