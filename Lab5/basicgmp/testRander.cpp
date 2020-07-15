#include <iostream>
#include <bignum.h>
#include <random>
#include <time.h>
#include <rander.h>
#include <math.h>
using namespace std;

int seed;
Rander rander;

int range = 1000;
int counter[100002];
double p[100002];
int nrand = 1000000;

void testRandomm()
{
    seed = rand() % 100000;
    range = 1000;

    rander.initSeed(seed);
    bignum val = 1;
    for (int t=1; t<=nrand; t++) {
        val = rander.randomm(range);
        counter[(int)val]++;
    }

    for (int i=0; i<range; i++) p[i] = double(counter[i])/nrand;
    double mean = 1 / range;
    double variance = 0, maxDiff = 0;
    for (int i=0; i<range; i++) {
        variance += (p[i]-mean)*(p[i]-mean);
        maxDiff = max(maxDiff, fabs(p[i] - mean));
    }

    cout << "Randomm (0 -> n - 1) results:\n";
    cout << "variance = " << variance << "\n";
    cout << "max distant from mean = " << maxDiff << "\n\n\n";
}

void testRandomb()
{
    seed = rand() % 100000;
    int kbit = 10;
    range = 1024;

    rander.initSeed(seed);
    bignum val = 1;
    for (int t=1; t<=nrand; t++) {
        val = rander.randomb(10);
        counter[(int)val]++;
    }

    for (int i=0; i<range; i++) p[i] = double(counter[i])/nrand;
    double mean = 1 / range;
    double variance = 0, maxDiff = 0;
    for (int i=0; i<range; i++) {
        variance += (p[i]-mean)*(p[i]-mean);
        maxDiff = max(maxDiff, fabs(p[i] - mean));
    }

    cout << "Randomb(0 -> 2^n - 1) results:\n";
    cout << "variance = " << variance << "\n";
    cout << "max distant from mean = " << maxDiff << "\n\n\n";
}

int main()
{
    srand(time(NULL));

    testRandomm();
    testRandomb();
    getchar();
}
