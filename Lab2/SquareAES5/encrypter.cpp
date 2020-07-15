#include <stdio.h>
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

byte KEY_ARR[16];
byte expanded_key[176];

const byte round5key[16] = {128, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 5};
byte MAX_KEY;
vector<byte> mainKey;
vector<byte> expandedKey;

void init()
{
    MAX_KEY = round5key[0];
    for (int i=1; i<16; i++) MAX_KEY = max(MAX_KEY, round5key[i]);
    MAX_KEY += 1;

    vector<byte> lastKey, allKeys;
    for (int i=0; i<16; i++) lastKey.push_back(round5key[i]);
    invertKeyExpansion(allKeys, lastKey, 5);

    for (int i=0; i<16; i++) {
        KEY_ARR[i] = allKeys[i];
        mainKey.push_back(allKeys[i]);
    }

    key_schedule(expanded_key, KEY_ARR);
    expandedKey.clear();
    for (int i=0; i<176; i++) expandedKey.push_back(expanded_key[i]);
}

int main()
{
    int ntest, k;

    init();

    //-----------
    freopen("plaint.txt", "r", stdin);
    freopen("encrypted.txt", "w", stdout);

    // - first line of encrypted.txt is number of delta-set: we only need 5 but we use 10 delta-set just to be sure

    // - the second line contains 1 number: the maximum key value to search. Key values will only be in range 0->MAX_KEY-1 instead of 0->255.
    //   This is a "hint" so the decrypter will only search this range -> find the result faster.
    //   No hint means MAX_KEY == 256. You can write code to change this value.

    // - The third line contains 4 numbers: 4 bytes of the last row of round 5 key, from column0 -> column3
    //   Searching for 256^5 keys takes too long, but searching 256^4 keys can be done in ~~12 hours when using single-threaded code.
    //   We can use Nvidia CUDA to solve this problem in 10 seconds. But I will implement that in another course.

    // - Finally, next ntest lines, each line is a delta-set as described in DeltaGenerator.cpp
    scanf("%d", &ntest);

    printf("%d\n", ntest);
    printf("%d\n", MAX_KEY);
    for (int col=0; col<4; col++) printf("%d ", round5key[cell(3, col)]);
    printf("\n");


    vector<byte> message, cipher;
    message.resize(16);
    cipher.resize(16);

    for (int t=0; t<ntest; t++) {
        for (int i=0; i<256; i++)
        {
            for (int j=0; j<16; j++) scanf("%d", &message[j]);

            aesEncryptVector(cipher, message, expandedKey, 5);

            for (int j=0; j<16; j++) printf("%d ",cipher[j]);
        }
        printf("\n");
    }

    fclose(stdin);
    fclose(stdout);
    return 0;
}
