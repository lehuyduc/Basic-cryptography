#include <stdio.h>
#include <iomanip>      // std::setw
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include "aes.h"
using namespace std;

typedef unsigned char byte;
typedef unsigned int uint;

void makeDeltaSet(vector<vector<byte>> &plaintexts, int pos) // active pos
{
    int i, j;
    static vector<int> rand256;
    if (rand256.size() != 256) {
        rand256.resize(256);
        for (i=0; i<256; i++) rand256[i] = i;
    }
    random_shuffle(rand256.begin(), rand256.end());

    for (int j=0; j<16; j++) plaintexts[0][j] = rand() % 256;

    plaintexts[0][pos] = rand256[0];
    for (i=1; i<256; i++) {
        for (j=0; j<16; j++)
            plaintexts[i][j] = plaintexts[i-1][j];
        plaintexts[i][pos] = rand256[i];
    }
}

int main()
{
    int ntest;
    vector<vector<byte>> plaintexts;

    printf("Enter number of Delta-set you want to make (ntest): ");
    scanf("%d", &ntest);

    //-----
    // the first line of the file is the number of delta-set: ntest
    // the next ntest lines, each line contain 256*16 value == 256 plaintexts, each plaintext has 16 bytes
    // The first 16 number is plaintexts[0], next 16 is plaintexts[1], ...
    freopen("plaint.txt", "w", stdout);
    printf("%d\n", ntest);

    plaintexts.resize(256);
    for (int i=0; i<256; i++) plaintexts[i].resize(16);

    for (int t=1; t<=ntest; t++) {
        makeDeltaSet(plaintexts, rand() % 16);
        for (int i=0; i<256; i++)
            for (int j=0; j<16; j++) printf("%d ",plaintexts[i][j]);
        printf("\n");
    }

    fclose(stdin);
    return 0;
}



