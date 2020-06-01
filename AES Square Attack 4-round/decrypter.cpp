#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <stdlib.h>
#include <cstring>
#include <time.h>

#include "aes.h"

using namespace std;

typedef unsigned char byte;
typedef unsigned int uint;

vector<byte> mainKey;
vector<byte> expandedKey;

int ntest;
vector<vector<byte>> cipherList[22];

/*************************************/
bool checkValidKeyGuess(int row, int col, byte guess)
{
    vector<byte> reversed256;
    reversed256.resize(256);
    int pos = cell(row,col);

    for (int t=0; t<ntest; t++) {
        vector<vector<byte>> *deltaSet = &cipherList[t];
        for (int i=0; i<256; i++) reversed256[i] = subByteValue((*deltaSet)[i][pos] ^ guess, DECRYPT);  // reversed256[i] is the cell at pos (row,col) in ciphertext[i] at the end of round 3.

        byte xor256 = 0;
        for (int i=0; i<256; i++) xor256 ^= reversed256[i];
        if (xor256!=0) return false;    // check balanced property
    }

    // if pass all tests, then assume it's the correct key guess
    return true;
}


int main()
{
    initPrecomputeAES();

    int k;
    ifstream fin("encrypted.txt");
    fin >> ntest;

    for (int t=0; t<ntest; t++) {
        cipherList[t].resize(256);
        for (int i=0; i<256; i++) {
            cipherList[t][i].resize(16);
            for (int j=0; j<16; j++) {
                fin >> k;
                cipherList[t][i][j] = k;
            }
        }
    }
    fin.close();

    /****************************/
    vector<byte> lastKey;
    lastKey.resize(16);

    for (int row=0; row<4; row++)
    for (int col=0; col<4; col++) {
        for (int guess=0; guess<=255; guess++) if (checkValidKeyGuess(row, col, guess)) {
            lastKey[cell(row,col)] = guess;
            break;
        }
    }

    cout << "*****************************\n";

    cout << "round 4 key = ";
    for (int i=0; i<16; i++) cout << int(lastKey[i]) << " ";
    cout << "\n\n";

    vector<byte> secretKeys;
    invertKeyExpansion(secretKeys, lastKey, 4);

    cout << "secret key = ";
    for (int i=0; i<16; i++) cout << int(secretKeys[i]) << " ";
    cout << "\n";

    /******************************/

    ofstream fo("mainkeyGuess.txt");
    for (int i=0; i<16; i++) fo << int(secretKeys[i]) << " ";
    fo << "\n";

    cout << "Finished decrypting successfully\n";
    cout << "*****************************\n";
    return 0;
}


