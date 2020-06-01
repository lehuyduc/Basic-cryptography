#include <iostream>
#include <algorithm>    // std::reverse
#include <string>
#include <vector>
#include "bignum.h"
#include "lib.h"
#include "rander.h"
using namespace std;

// In GMP, bit 0 is the most significant bit.
// In the lecture slides, bit 0 is the least significant bit
// So we reverse the binary string to make it consistent with the algorithms in the slide
bignum powermodLrsm(bignum m, bignum dkey, bignum n)        // Lrsm = left-right square multiply
{
    // value 2 means base-2
    string d = dkey.toString(2);     // d[0] is most significant bit in gmp
    reverse(d.begin(), d.end());     // d[0] is least significant bit after reverse.
    int k = d.length();

    bignum a = 1;
    for (int i=k-1; i>=0; i--) {
        a = sqr(a) % n;
        if (d[i]=='1')
            a = a*m % n;
    }
    return a;
}

bignum powermodRlsm(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);
    reverse(d.begin(), d.end());
    int k = d.length();

    bignum a = 1, b  = m;
    for (int i=0; i<=k-1; i++) {
        if (d[i]=='1') a = a*b % n;
        b = sqr(b) % n;
    }
    return a;
}


vector<int> sdr2naf(bignum d)
{
    vector<int> res;
    res.reserve(d.getSize(2) + 1);  // NAF might have extra 1 bit compared to regular binary form

    int i = 0;
    while (d >= 1) {
        if (d%2==1) {
            res.push_back(int(2 - (d % 4)));
            d = d - res[i];
        }
        else
            res.push_back(0);
        d /= 2;
        i++;
    }

    return res;
}

bignum powermodSd(bignum m, bignum dkey, bignum n)
{
    vector<int> d = sdr2naf(dkey);
    int k = d.size();

    bignum a = 1;
    bignum minv = inverse(m, n);
    for (int i=k-1; i>=0; i--) {
        a = sqr(a) % n;
        if (d[i]==1)
            a = a*m % n;
        if (d[i]==-1)
            a = a*minv % n;
    }
    return a;
}

// A means Always
bignum powermodLrsmA(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);    // d[0] is most significant bit in gmp
    reverse(d.begin(), d.end());    // d[0] is least significant bit after reverse.
    int k = d.length();

    bignum a = 1, b = 1;
    for (int i=k-1; i>=0; i--) {
        a = sqr(a) % n;
        if (d[i]=='1')
            a = a*m % n;
        else
            b = a*m % n;
    }
    return a;
}

bignum powermodRlsmA(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);
    reverse(d.begin(), d.end());
    int k = d.length();

    bignum a=1, b=1, c=m;
    for (int i=0; i<=k-1; i++) {
        if (d[i]=='1')
            a = a*c % n;
        else
            b = b*c % n;
        c = sqr(c) % n;
    }
    return a;
}

bignum powermodMontgo(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);
    reverse(d.begin(), d.end());
    int k = d.length();

    bignum R[2];
    R[0] = 1;
    R[1] = m;
    for (int i=k-1; i>=0; i--) {
        int di = d[i] - '0';            // d[i]=='0' -> di==0, d[i]=='1' -> di = 1
        R[1-di] = R[0] * R[1] % n;
        R[di] = sqr(R[di]) % n;
    }
    return R[0];
}

bignum powermodJoye(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);
    reverse(d.begin(), d.end());
    int k = d.length();

    bignum R[2];
    R[0] = 1;
    R[1] = m;
    for (int i=0; i<=k-1; i++) {
        int di = d[i] - '0';
        R[1-di] = (sqr(R[1-di]) * R[di]) % n;
    }
    return R[0];
}

bool testAccuracy(int ntest, int kbit_length)
{
    bignum m, d, n;
    Rander rander(time(NULL));
    bignum res[8];
    int nalgos = 8;

    for (int t=1; t<=ntest; t++) {
        d = rander.getKbitNum(kbit_length);
        n = d + rander.randomm(d);
        m = rander.randomm(n);
        while (gcd(m,n) != 1) m++;      // we need to ensure m and n are coprime so that m^-1 (mod n) exist

        res[0] = powermod(m,d,n);
        res[1] = powermodLrsm(m,d,n);
        res[2] = powermodRlsm(m,d,n);
        res[3] = powermodSd(m,d,n);
        res[4] = powermodLrsmA(m,d,n);
        res[5] = powermodRlsmA(m,d,n);
        res[6] = powermodMontgo(m,d,n);
        res[7] = powermodJoye(m,d,n);

        bool ok = true;
        for (int i=1; i<nalgos; i++)
            if (res[i]!=res[i-1]) ok = false;

        if (!ok) {
            cout << "Test Accuracy failed. Please check\n";
            exit(1);
        }
        if (t%2==0) cout << (double(t)/ntest*100) << "% completed\n";
    }
    cout << "Test Accuracy success\n";
}

int kbit_length;
int ntest;

int main(int argc, char* argv[])
{
    kbit_length = 1024;
    ntest = 100;
    if (argc >= 2) kbit_length = atoi(argv[1]);
    if (argc >= 3) ntest = atoi(argv[2]);

    testAccuracy(ntest, kbit_length);

    return 0;
}
