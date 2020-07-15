#include <bits/stdc++.h>
using namespace std;

int guess[4];
int main()
{
    clock_t st, ed;
    double timer;

    st = clock();
    for (int i=0; i<256; i++)
        for (int j=0; j<256;j++)
            for (int k=0; k<256; k++)
        for (int g=0; g<256; g++) {
            guess[0] = i;
            guess[1] = j;
            guess[2] = k;
            guess[3] = g;
        }
    ed = clock();
    timer = double(ed-st) / CLOCKS_PER_SEC;
    cout << timer << "\n";

    st = clock();
    for (int i=0; i<256; i++) {
        guess[0] = i;
        for (int j=0; j<256; j++) {
            guess[1] = j;
            for (int k=0; k<256; k++) {
                guess[2] = k;
                for (int g=0; g<256; g++) {
                    guess[3] = g;
                }
            }
        }
    }
    ed = clock();
    timer = double(ed-st) / CLOCKS_PER_SEC;
    cout << timer << "\n";

    return 0;
}
