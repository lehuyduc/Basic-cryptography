#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "bignum.h"
#include "lib.h"
#include "rander.h"
using namespace std;

bool testGenericSpeed(int ntest, int kbit_length);
bool testExpoSpeed(int ntest, int kbit_length);

int kbit_length;
int ntest;

int main(int argc, char* argv[])
{
    kbit_length = 2048;
    ntest = 100;
    if (argc >= 2) kbit_length = atoi(argv[1]);
    if (argc >= 3) ntest = atoi(argv[2]);

    cout << "Start Exponentiation test: \n";
    testExpoSpeed(ntest, kbit_length);
    cout << "\n\n";

    cout << "Start Generic calculation test: \n";
    testGenericSpeed(ntest, kbit_length);
    return 0;
}


/****************************/
// In this test, we compare the speed of bignum.h and gmp.h
// The expression we calculate is: res = (1 + (m*d%n - 4*n) / (m*d/2 % (n/4))) * x1 + x1;
bool testGenericSpeed(int ntest, int kbit_length)
{
    bignum m, d, n, res, res3;
    mpz_t mt, dt, nt, rest;
    bignum x1;
    mpz_t tmp1, tmp2, tmp3, tmp4;
    Rander rander;
    double bignumTime = 0, mpzTime = 0;
    clock_t start1, end1, start2, end2;

    mpz_init(mt);
    mpz_init(dt);
    mpz_init(nt);
    mpz_init(rest);
    mpz_init(tmp1);
    mpz_init(tmp2);
    mpz_init(tmp3);
    mpz_init(tmp4);

    for (int t=1; t<=ntest; t++) {
        d = rander.getKbitNum(kbit_length);
        n = d + rander.randomm(d);
        m = rander.randomm(n);

        bignum::setmpz(mt, m);
        bignum::setmpz(dt, d);
        bignum::setmpz(nt, n);

        //---
        start1 = clock();
        x1 = powermod(m, d, n);
        res = (1 + (m*d%n - 4*n) / (m*d/2 % (n/4))) * x1 + x1;
        end1 = clock();
        bignumTime += double(end1 - start1) / double(CLOCKS_PER_SEC);

        //---
        start2 = clock();

        mpz_mul(rest, mt, dt);        // res  = m*d
        mpz_tdiv_r(rest, rest, nt);   // res  = m*d % n
        mpz_mul_ui(tmp1, nt, 4);      // tmp1 = 4*n
        mpz_sub(rest, rest, tmp1);    // res  = m*d%n - 4*n

        mpz_mul(tmp2, mt, dt);        // tmp2 = m*d
        mpz_tdiv_q_ui(tmp2, tmp2, 2); // tmp2 = m*d/2
        mpz_tdiv_q_ui(tmp3, nt, 4);   // tmp3 = n/4
        mpz_tdiv_r(tmp2, tmp2, tmp3); // tmp2 = m*d/2 % (n/4)

        mpz_tdiv_q(rest, rest, tmp2); // res  = (m*d%n - 4*n) / (m*d/2 % (n/4))
        mpz_add_ui(rest, rest, 1);    // res  = 1 + (m*d%n - 4*n) / (m*d/2 % (n/4))

        mpz_powm(tmp4, mt, dt, nt);   // tmp4 = m^d % n == x1
        mpz_mul(rest, rest, tmp4);    // res  = 1 + (m*d%n - 4*n) / (m*d/2 % (n/4)) * x1
        mpz_add(rest, rest, tmp4);    // res  = 1 + (m*d%n - 4*n) / (m*d/2 % (n/4)) * x1 + x1

        end2 = clock();
        mpzTime += double(end2 - start2) / double(CLOCKS_PER_SEC);

        if (res!=bignum(rest)) {
            cout << "Failed. Please debug\n";
            cout << res << "\n";
            return 0;
        }

        if (t%10==0) cout << (double(t)/ntest*100) << "% completed\n";
    }

    cout << "Test Generic Speed success\n";
    cout << "Time using my bignum.h          : " << bignumTime << "\n";
    cout << "Time using gmp mpz_t            : " << mpzTime << "\n";
}

// In this test, we generate m,d,n and compute (m^d mod n)
// using bignum.h and mpz_t
bool testExpoSpeed(int ntest, int kbit_length)
{
    bignum m, d, n, res, res3;
    mpz_t mt, dt, nt, rest;
    Rander rander;
    double bignumTime = 0, mpzTime = 0;
    clock_t start1, end1, start2, end2;

    mpz_init(mt);
    mpz_init(dt);
    mpz_init(nt);
    mpz_init(rest);

    for (int t=1; t<=ntest; t++) {
        d = rander.getKbitNum(kbit_length);
        n = d + rander.randomm(d);
        m = rander.randomm(n);

        bignum::setmpz(mt, m);
        bignum::setmpz(dt, d);
        bignum::setmpz(nt, n);

        //---
        start1 = clock();
        res = powermod(m, d, n);
        end1 = clock();
        bignumTime += double(end1 - start1) / double(CLOCKS_PER_SEC);

        //---
        start2 = clock();
        mpz_powm(rest, mt, dt, nt);
        end2 = clock();
        mpzTime += double(end2 - start2) / double(CLOCKS_PER_SEC);

        if (res!=bignum(rest)) {
            cout << "Failed. Please debug\n";
            cout << res << "\n";
            return 0;
        }

        if (t%2==0) cout << (double(t)/ntest*100) << "% completed\n";
    }

    cout << "Test Exponentiation Speed success\n";
    cout << "Time using my bignum.h          : " << bignumTime << "\n";
    cout << "Time using gmp mpz_t            : " << mpzTime << "\n";
}
