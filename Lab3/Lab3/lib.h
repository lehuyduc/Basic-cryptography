#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <stdlib.h>
using namespace std;

#define X first
#define Y second

typedef long long ll;
typedef pair<ll, ll> pt;

const int maxn = 10000000;

bool inited = 0;
bool isPrime[maxn+2];
int phi[maxn+2];
vector<ll> primes;

ll gcd(ll a,ll b) {
    while (b) {
        a %= b;
        swap(a,b);
    }
    return a;
}

bool checkPrimeSlow(ll n)
{
    if (n<=1) return false;
    int lim = sqrt(n);
    for (int i=2; i<=lim; i++) if (n%i==0) return false;
    return true;
}

int eulerPhiSlow(int n)
{
    int res = 0;
    for (int i=1; i<n; i++) if (gcd(n,i)==1) res++;
    return res;
}

ll inverseSlow(ll a, ll base)
{
    a %= base;
    for (ll i=1; i<base; i++) if (a*i % base == 1) return i;
    return -1;
}

//---------------
ll power(ll x,int n) {
    if (n==0) return 1;
    ll j = power(x, n/2);
    if (n%2 == 0) return j*j;
    else return j*j*x;
}

ll powermod(ll x, int n, ll base) {
    if (n==0) return 1;
    ll j = powermod(x, n/2, base);
    if (n%2 == 0) return j*j%base;
    else return ((j*j) % base) * x % base;
}

void eratos()
{
    int n = maxn;
    isPrime[0] = 0;
    isPrime[1] = 0;
    for (int i=2;i<=n;i++) isPrime[i] = true;

    int lim = sqrt(n);
    for (int i=2; i<=lim; i++) {
        if (isPrime[i]) {
            for (ll j=i*i; j<=n; j+=i) isPrime[j] = false;
        }
    }

    for (int i=2; i<=n; i++) if (isPrime[i]) primes.push_back(i);
    inited = true;
}

void init()
{
    if (inited) return;
    eratos();

    phi[0] = 0;
    phi[1] = 0;
    for (int i=2; i<=maxn; i++) phi[i] = -1;
}

bool checkPrime(int n)
{
    if (!inited) init();
    return isPrime[n];
}

// phi of p^k, where p is a prime
int phiPrime(ll p, int k) {
    return (p-1)*power(p, k-1);
}

int eulerPhi(int n)
{
    if (!inited) init();
    if (isPrime[n]) return n-1;

    int res = 1, k;
    for (int i=0; i<primes.size(); i++) {
        int prime = primes[i];

        if (n % prime==0) {
            k = 0;
            while (n % prime==0) {
                k++;
                n /= prime;
            }

            res *= phiPrime(prime, k);
        }

        if (prime*prime > n) break;
        if (n==1) break;
    }

    if (n > 1) res *= phiPrime(n, 1);
    return res;
}

pt extendedGcd(ll a, ll b) {
    if (b==0) return pt(1, 0);
    pt qr, st;

    qr = pt(a/b, a%b);
    st = extendedGcd(b, qr.Y);
    return pt(st.Y, st.X - qr.X * st.Y);
}

ll inverse(ll a, ll base)
{
    pt res = extendedGcd(a, base);
    return (res.X + base) % base;
}

/***********************************************/
void testPrime(int range)
{
    for (int i=0; i<=range; i++) if (checkPrime(i) != checkPrimeSlow(i))
        {cout << "test prime failed at " << i << "\n"; return;}
    cout << "test prime succ\n";
}

void testPhi(int range)
{
    for (int i=0; i<=range; i++) if (eulerPhi(i) != eulerPhiSlow(i))
        cout << "test phi failed at " << i << "\n";
    cout << "test phi succ\n";
}
