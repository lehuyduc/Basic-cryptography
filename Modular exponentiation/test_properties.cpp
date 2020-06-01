#include <iostream>
#include <algorithm>    // std::reverse
#include <string>
#include <vector>
#include <iomanip>
#include "bignum.h"
#include "lib.h"
#include "rander.h"
using namespace std;

// exponentiation result:
// value: result of the computation
// relation: true if the special relation is correct
struct expores {
    bignum value;
    int sqrCount, mulCount;
    bool relation;

    expores() {value = 0; sqrCount = 0; mulCount = 0; relation = 0;}
    expores(bignum a, int b, int c, bool d) {value = a; sqrCount = b; mulCount = c; relation = d;}
};

const bool checkingRelation = true;     /**** CHECK CONSISTENCY RELATIONS OF REGISTER    ***/

expores powermodLrsm(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);     // d[0] is most significant bit
    reverse(d.begin(), d.end());     // d[0] is least significant bit after reverse
    int k = d.length();
    int sqrCount = 0, mulCount = 0;
    bool relation = true;

    bignum a = 1;
    for (int i=k-1; i>=0; i--) {
        a = sqr(a) % n;
        sqrCount++;
        if (d[i]=='1') {
            a = a*m % n;
            mulCount++;
        }

        if (checkingRelation) {
            string dtmp = d.substr(i);         // substring d[i->k-1]
            reverse(dtmp.begin(), dtmp.end()); // constructor bignum(string, 2) requires bit 0 is most significant
            if (a != powermod(m, bignum(dtmp,2), n)) relation = false;
        }
    }

    return expores(a, sqrCount, mulCount, relation);
}

expores powermodRlsm(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);
    reverse(d.begin(), d.end());
    int k = d.length();
    int sqrCount = 0, mulCount = 0;
    bool relation = true;

    bignum a = 1, b  = m;
    for (int i=0; i<=k-1; i++) {
        if (d[i]=='1') {
            a = a*b % n;
            mulCount++;
        }
        b = sqr(b) % n;
        sqrCount++;

        if (checkingRelation) {
            string dtmp = d.substr(0,i+1);      // d[0->i]
            reverse(dtmp.begin(), dtmp.end());
            if (a != powermod(m, bignum(dtmp,2), n)) relation = false;
        }
    }

    return expores(a, sqrCount, mulCount, relation);
}


vector<int> sdr2naf(bignum d)
{
    vector<int> res;
    res.reserve(d.getSize(2) + 1);

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

expores powermodSd(bignum m, bignum dkey, bignum n)
{
    vector<int> d = sdr2naf(dkey);
    int k = d.size();
    int sqrCount = 0, mulCount = 0;
    bool relation = true;

    bignum a = 1;
    bignum minv = inverse(m, n);
    for (int i=k-1; i>=0; i--) {
        a = sqr(a) % n;
        sqrCount++;
        if (d[i]==1)  {a = a*m % n;     mulCount++;}
        if (d[i]==-1) {a = a*minv % n;  mulCount++;}
    }

    return expores(a, sqrCount, mulCount, relation);
}

expores powermodLrsmA(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);        // d[0] is most significant bit
    reverse(d.begin(), d.end());        // d[0] is least significant bit after reverse
    int k = d.length();
    int sqrCount = 0, mulCount = 0;
    bool relation = true;

    bignum a = 1, b = 1;
    for (int i=k-1; i>=0; i--) {
        a = sqr(a) % n;
        sqrCount++;
        if (d[i]=='1')
            a = a*m % n;
        else
            b = a*m % n;
        mulCount++;
    }

    return expores(a, sqrCount, mulCount, relation);
}

expores powermodRlsmA(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);
    reverse(d.begin(), d.end());
    int k = d.length();
    int sqrCount = 0, mulCount = 0;
    bool relation = true;

    bignum a=1, b=1, c=m;
    for (int i=0; i<=k-1; i++) {
        if (d[i]=='1')
            a = a*c % n;
        else
            b = b*c % n;
        mulCount++;
        c = sqr(c) % n;
        sqrCount++;
    }

    return expores(a, sqrCount, mulCount, relation);
}

expores powermodMontgo(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);
    reverse(d.begin(), d.end());
    int k = d.length();
    int sqrCount = 0, mulCount = 0;
    bool relation = true;

    bignum R[2];
    R[0] = 1;
    R[1] = m;
    for (int i=k-1; i>=0; i--) {
        int di = d[i] - '0';
        R[1-di] = R[0] * R[1] % n; mulCount++;
        R[di] = sqr(R[di]) % n;    sqrCount++;

        if (checkingRelation) {
            string dtmp = d.substr(i);  // d[i->k-1]
            reverse(dtmp.begin(), dtmp.end());
            bignum R0tmp = powermod(m, bignum(dtmp, 2), n);
            bignum R1tmp = R0tmp * m % n;
            if (R[0] != R0tmp || R[1] != R1tmp) relation = false;
        }
    }

    return expores(R[0], sqrCount, mulCount, relation);
}

expores powermodJoye(bignum m, bignum dkey, bignum n)
{
    string d = dkey.toString(2);
    reverse(d.begin(), d.end());
    int k = d.length();
    int sqrCount = 0, mulCount = 0;
    bool relation = true;

    bignum R[2];
    R[0] = 1;
    R[1] = m;
    for (int i=0; i<=k-1; i++) {
        int di = d[i] - '0';
        R[1-di] = (sqr(R[1-di]) * R[di]) % n;
        sqrCount++;
        mulCount++;

        if (checkingRelation) {
            string dtmp = d.substr(0, i+1);    // d[0->i]
            reverse(dtmp.begin(), dtmp.end());
            bignum R0tmp = powermod(m, bignum(dtmp,2), n);
            bignum R0R1 = powermod(m, power(2, i+1), n);

            if (R[0] != R0tmp) relation = false;
            if (R[0]*R[1]%n != R0R1) relation = false;
        }
    }

    return expores(R[0], sqrCount, mulCount, relation);
}

bool testProperties(int ntest, int kbit_length)
{
    bignum m, d, n;
    Rander rander(time(NULL));
    expores res[8];
    expores counter[8]; // store the total amount of square/multiplication count for each algorithm
    vector<string> algos;

    int nalgos = 7;
    algos.resize(7);
    algos[0] = "Left-Right Square Multiply";
    algos[1] = "Right-left Square Multiply";
    algos[2] = "Left-Right Signed Digit Square Multiply";
    algos[3] = "Left-Right Square Multiply Always";
    algos[4] = "Right-Left Square Multiply Always";
    algos[5] = "Montgomery Ladder";
    algos[6] = "Joye Ladder";

    for (int i=0; i<nalgos; i++) {
        counter[i].sqrCount = 0;
        counter[i].mulCount = 0;
    }

    /**------------------------------**/

    for (int t=1; t<=ntest; t++) {
        d = rander.getKbitNum(kbit_length);
        n = d + rander.randomm(d);
        m = rander.randomm(n);
        while (gcd(m,n) != 1) m++;


        res[0] = powermodLrsm(m,d,n);
        res[1] = powermodRlsm(m,d,n);
        res[2] = powermodSd(m,d,n);
        res[3] = powermodLrsmA(m,d,n);
        res[4] = powermodRlsmA(m,d,n);
        res[5] = powermodMontgo(m,d,n);
        res[6] = powermodJoye(m,d,n);

        bool ok = true;
        for (int i=0; i<nalgos; i++) {
            if (i>0 && res[i].value!=res[i-1].value) ok = false;
            counter[i].sqrCount += res[i].sqrCount;
            counter[i].mulCount += res[i].mulCount;
        }

        if (!ok) {
            cout << "Accuracy failed. Please check\n";
            exit(1);
        }
        for (int i=0; i<nalgos; i++)
            if (!res[i].relation) {
                cout << algos[i] << " register relation failed. Please check\n";
                exit(1);
            }

        if (t%2==0) cout << (double(t)/ntest*100) << "% completed\n";
    }

    cout << "\nAccuracy is good\n";
    cout << "Register relations are good\n\n";

    cout << "Average number of square/multiply for " << kbit_length << " bits private key:\n";
    cout << std::left;
    cout << setw(45) << "Algorithm" << setw(22) << "Square count" << std::setw(22) << "Multiplication count"; cout << "\n\n";

    for (int i=0; i<nalgos; i++)
        cout << setw(45) << algos[i] << setw(22) << (double(counter[i].sqrCount)/ntest) << setw(22) << (double(counter[i].mulCount)/ntest) << "\n";

}

int kbit_length;
int ntest;

int main(int argc, char* argv[])
{
    kbit_length = 512;
    ntest = 100;
    if (argc >= 2) kbit_length = atoi(argv[1]);
    if (argc >= 3) ntest = atoi(argv[2]);

    testProperties(ntest, kbit_length);
    return 0;
}

