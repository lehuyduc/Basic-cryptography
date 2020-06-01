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

byte round5key[16] = {32, 31, 3, 28, 21, 27, 32, 31, 19, 35, 11, 15, 20, 27, 3, 5}; // 32 13 3 28 21 27 32 31 19 35 11 15 20 27 3 5

byte MAX_KEY;
vector<byte> mainKey;
vector<byte> expandedKey;

void init()
{
    // we use round-5 key as input instead of main key. This is intentional and not because of technical difficulty. Explanation in the report
    ifstream fin("key5.txt");
    for (int i=0; i<16; i++) {
        int key5IthByte;
        fin >> key5IthByte;
        round5key[i] = key5IthByte;
    }

    // MAX_KEY is like a hint. Decrypter.cpp only search values from 0 -> MAX_KEY
    MAX_KEY = round5key[0];
    for (int i=1; i<16; i++) MAX_KEY = max(MAX_KEY, round5key[i]);

    // get all keys (round 0 -> round 5) from last round key
    vector<byte> lastKey;
    for (int i=0; i<16; i++) lastKey.push_back(round5key[i]);
    invertKeyExpansion(expandedKey, lastKey, 5);

    for (int round=0; round<=5; round++) {
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

    // - first line of encrypted.txt is number of delta-set: we need 5 on average so we use 10 delta-set just to be sure

    // - the second line contains 1 number: the maximum key value to search. Key values will only be in range 0->MAX_KEY instead of 0->255.
    //   This is a "hint" so the decrypter will only search this range -> find the result faster.
    //   No hint means MAX_KEY == 255. You can write code to change this value.

    // - The third line contains 4 numbers: 4 bytes of the last row of round 5 key, from column0 -> column3
    //   Searching for 256^5 keys takes too long, but searching 256^4 keys + testing can be done in ~~9 hours when using single-threaded code.
    //   We can use Nvidia CUDA to solve this problem in 10 seconds since testing different keys is 100% parallel. But I will implement that in another course.

    // - Finally, next ntest lines, each line is a delta-set as described in DeltaGenerator.cpp
    fin >> ntest;

    fo << ntest << "\n";
    fo << int(MAX_KEY) << "\n";
    for (int col=0; col<4; col++) fo << int(round5key[cell(3,col)]) << " "; // give 4 bytes of the last row as a hint
    fo << "\n";

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

            aesEncryptVector(cipher, message, expandedKey, 5);

            for (int j=0; j<16; j++) fo << int(cipher[j]) << " ";
        }
        fo << "\n";
    }

    fin.close();
    fo.close();

    cout << "Finished encrypting successfully\n\n*************************\n";
    return 0;
}
