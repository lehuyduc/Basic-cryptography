#include <iostream>
#include <bignum.h>
#include <random>
#include <time.h>
using namespace std;

template<class T>
T calc(const T &a, const T &b, char op)
{
    if (op=='+') return a+b;
    if (op=='-') return a-b;
    if (op=='*') return a*b;
    if (op=='/') {if (b==0) return a; else return a/b;}
    cout << "Error at calc: " << op << "\n";
}
int bignum::destroyed = 0;

void testBignum()
{
    vector<int> a, res1;
    vector<bignum> b, res2;
    vector<char> ops;
    const char opera[4] = {'+','-','*','/'};

    for (int t=1; t<= 1000000; t++)
    {
        a.clear();
        b.clear();
        res1.clear();
        res2.clear();
        ops.clear();
        ops.push_back(' ');

        int n = rand()%6+1;
        for (int i=1; i<=n; i++) {
            int val = rand() % 32;
            a.push_back(val);
            b.emplace_back(bignum(val));
            if (i<n) ops.push_back(opera[rand()%4]);
        }

        res1.push_back(a[0]);
        res2.emplace_back(b[0]);
        for (int i=1; i<n; i++) {
            res1.push_back(calc(res1[i-1], a[i], ops[i]));
            res2.emplace_back(bignum(calc(res2[i-1], b[i], ops[i])));
        }

        if (res2[n-1] != res1[n-1]) {
            for (int i=0; i<n; i++) cout << a[i] << " "; cout << "\n";
            for (int i=0; i<n; i++) cout << res1[i] << " "; cout << "\n";
            for (int i=0; i<n; i++) cout << (b[i]) << " "; cout << "\n";
            for (int i=0; i<n; i++) cout << (res2[i]) << " "; cout << "\n";
            for (int i=0; i<n; i++) cout << ops[i] << " "; cout << "\n";
            return;
        }

        if (t % 1000 == 0) cout << t << "\n";
    }

    cout << "bignum.h correct\n";
}

int main()
{
    testBignum();
}
