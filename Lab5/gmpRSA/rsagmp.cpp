#include <iostream>
#include <bignum.h>
#include <fstream>
#include <random>
#include <rander.h>
#include <lib.h>
using namespace std;

Rander rander;
int nPrime;
bignum primes[100];

void init();
void testRsa(int ntest);
void testGUI(int ntest);

bignum getKbitNum(Rander &rander, int kbits)
{
    string res = "1";
    for (int i=2; i<=kbits; i++)
        res += char(rander.randomm(2) + '0');
    return bignum(res.c_str(), 2);
}

bignum getKbitPrime(int kbits)
{
    bignum res, tmp;
    tmp = getKbitNum(rander, kbits);
    res = nextPrime(tmp);

    // prevent case where nextPrime has more than kbit
    if (res.getSize(2) > kbits) {
        tmp = power(2, kbits - 1); // smallest number that has kbits
        res = nextPrime(tmp);
    }

    return res;
}

bignum rsaEncrypt(bignum mess, bignum n, bignum e) {
    return powermod(mess, e, n);
}

bignum rsaDecrypt(bignum cipher, bignum n, bignum d) {
    return powermod(cipher, d, n);
}

bignum rsaDecryptCrt(bignum cipher, bignum d, bignum p, bignum q) {
    bignum dp, dq, mp, mq;
    dp = d % (p-1);                    // dp and dq should be saved so that it doesn't need to be recalculated
    dq = d % (q-1);                    // so the function should be rsaDecryptCrt(cipher, d, p, q, dp, dq) to increase speed
    mp = powermod(cipher % p, dp, p);
    mq = powermod(cipher % q, dq, q);
    return crt2(mp, mq, p, q);
}


int main()
{
    bignum k, e, n ;
    bignum p, q, d, phin;
    bignum m, c;

    //testRsa(1000);
    //testGUI(1000);

    cout << "input k and e: "; cin >> k >> e;

    if (k <= 6) {
        cout << "Please input k > 6"; return 0;
    }

    if (e%2==0 || e<3) {
        cout << "e must be odd and larger than 2"; return 0;
    }

    int tried = 0;
    phin = e;
    while (gcd(e, phin) != 1) {
        tried++;
        if (tried > 100000) {
            cout << "Too hard to find p and q for your k, e values. Please input another k,e";
            return 0;
        }

        p = getKbitPrime(int(k/2));
        q = getKbitPrime(int(k/2));
        while (p==q) q = getKbitPrime(int(k/2));
        phin = (p-1) * (q-1);
    }
    n = p*q;
    d = inverse(e, phin);

    m = rander.randomm(n);
    c = rsaEncrypt(m, n, e);
    bignum res1 = rsaDecrypt(c, n, d);
    bignum res2 = rsaDecryptCrt(c, d, p, q);

    cout << "p = " << p << "\n";
    cout << "q = " << q << "\n";
    cout << "n = " << n << "\n";
    cout << "e = " << e << "\n";
    cout << "d = " << d << "\n";

    cout << "message = " << m << "\n";
    cout << "cipher = " << c << "\n";
    cout << "decrypted normally = " << res1 << "\n";
    cout << "decrypted with CRT = " << res2  << "\n";
    return 0;
}

/*******************************************/
void testGUI(int ntest)
{
    bignum k, e, n ;
    bignum p, q, d, phin;
    bignum m, c;

    for (int t=1; t<=ntest; t++) {
        cout << t << "\n";

        k = rander.randomm(100) + 12; // p q must have minimum 3 bit
        e = rander.randomm("1000000") + 3;
        if (e % 2 == 0) e++;

        int tried = 0;
        phin = e;
        while (gcd(e, phin) != 1) {
            tried++;
            if (tried > 1000) {t--; continue;}
            p = getKbitPrime(int(k/2));
            q = getKbitPrime(int(k/2));
            while (p==q) q = getKbitPrime(int(k/2));
            phin = (p-1) * (q-1);
        }
        n = p*q;
        d = inverse(e, phin);
        c = rsaEncrypt(m, n, e);
        bignum res1 = rsaDecrypt(c, n, d);
        bignum res2 = rsaDecryptCrt(c, d, p, q);

        bool error = false;
        if (res1!=c) {
            cout << "Error1\n";
            error = true;
        }
        if (res2!=c) {
            cout << "Error2\n";
            error = true;
        }

        if (error) {
            ofstream fo("error.txt");
            fo << "k = " << k << ";\n";
            fo << "e = " << e << ";\n";
            fo << "p = " << p << ";\n";
            fo << "q = " << q << ";\n";
            fo << "n = " << n << ";\n";
            fo << "d = " << d << ";\n";
            fo << "m = " << m << ";\n";
            fo << "c = " << c << ";\n";
            fo << "res1 = " << res1 << ";\n";
            fo << "res2 = " << res2 << ";\n";
            fo.close();
            return;
        }

    }

    cout << "Test success\n";
}

void init()
{
    ifstream fin("primes.txt");
    fin >> nPrime;
    for (int i=0; i<nPrime; i++) fin >> primes[i];
    fin.close();
}

void testRsa(int ntest)
{
    bignum e, n, p, q, d, phin, m, c, res1, res2;
    init();

    for (int t=1; t<=ntest; t++) {
        cout << t << "\n";

        e = rander.randomm("100000000000") + 1;
        p = primes[rand() % nPrime];
        q = primes[rand() % nPrime];
        while (p==q) q = primes[rand() % nPrime];
        n = p*q;
        phin = (p-1) * (q-1);

        while (gcd(e, phin) != 1) e++;
        d = inverse(e, phin);

        m = rander.randomm(n);
        c = rsaEncrypt(m, n, e);
        res1 = rsaDecrypt(c, n, d);
        res2 = rsaDecryptCrt(c, d, p, q);

        bool err = false;
        if (res1!=m) {
            cout << "Error1\n";
            err = true;
        }
        if (res2!=m) {
            cout << "Error2\n";
            err = true;
            if (gcd(m,p)!=1 || gcd(m,q)!=1) err = false; // p and q are not true prime
        }

        if (err) {
            ofstream fo("error.txt");
            fo << "e = " << e << ";\n";
            fo << "p = " << p << ";\n";
            fo << "q = " << q << ";\n";
            fo << "n = " << n << ";\n";
            fo << "d = " << d << ";\n";
            fo << "m = " << m << ";\n";
            fo << "c = " << c << ";\n";
            fo << "res1 = " << res1 << ";\n";
            fo << "res2 = " << res2 << ";\n";
            fo.close();
            return;
        }
    }

    cout << "Test success\n";
}

