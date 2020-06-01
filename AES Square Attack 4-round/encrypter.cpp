#include <fstream>
#include <iomanip>      // std::setw
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

#include "aes.h"
using namespace std;

typedef unsigned char byte;
typedef unsigned int uint;

vector<byte> mainKey;
vector<byte> expandedKey;

void init()
{
    // mainkey.txt has 1 line, 16 numbers: the main key of AES
    ifstream fin("mainkey.txt");
    for (int i=0; i<16; i++) {
        int k;
        fin >> k;
        mainKey.push_back(k);
    }

    expandedKey.resize(176);
    key_schedule(&expandedKey[0], &mainKey[0]);

    for (int round=0; round<=4; round++) {
        cout << "key round " << round << ": ";
        for (int i=0; i<16; i++) cout << int(expandedKey[16*round + i]) << " ";
        cout << "\n";
    }
}

int main()
{
    initPrecomputeAES();
    init();

    //----------- READ DELTA-SET INPUT
    int ntest, k;
    ifstream fin("plaint.txt");
    ofstream fo("encrypted.txt");

    // - first line of encrypted.txt is number of delta-set: for AES4-, we need 3 on average but we use 6 delta-set just to be sure
    // - next ntest lines, each line is a delta-set as described in DeltaGenerator.cpp
    fin >> ntest;
    fo << ntest << "\n";

    vector<byte> message, cipher;
    message.resize(16);
    cipher.resize(16);

    for (int t=0; t<ntest; t++) {
        for (int i=0; i<256; i++)
        {
            for (int j=0; j<16; j++) {
                fin >> k;
                message[j] = k;
            }

            aesEncryptVector(cipher, message, expandedKey, 4);

            for (int j=0; j<16; j++) fo << int(cipher[j]) << " ";
        }
        fo << "\n";
    }

    fin.close();
    fo.close();

    cout << "Finished encrypting successfully\n\n*************************\n";
    return 0;
}
