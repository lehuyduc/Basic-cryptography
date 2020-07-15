#include <stdio.h>
#include <iomanip>      // std::setw
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <stdlib.h>
#include <cstring>
#include <map>
#include <set>
#include <time.h>

#include "aes.h"

using namespace std;

typedef unsigned char byte;
typedef unsigned int uint;
const int nbText = 256;
typedef map<int,vector<byte> > MapVectorByte ;

const bool debugAll = 1, debugMakeDelta = 0, debugSetup3 = 0;
const bool verbose = 0;

//byte KEY_ARR[16] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
byte KEY_ARR[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 15};
byte IV_ARR[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte expanded_key[176];

vector<byte> mainKey;
vector<byte> IV;

void init(bool randoming)
{
    if (randoming) {
        for (int i=0; i<16; i++) KEY_ARR[i] = rand() % 256;
    }

    //byte key5[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    byte key5[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    //byte key5[16] = {0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 5};
    vector<byte> lastKey;
    for (int i=0; i<16; i++) lastKey.push_back(key5[i]);
    invertKeyExpansion(mainKey, lastKey, 5);
    for (int i=0; i<16; i++) KEY_ARR[i] = mainKey[i];

    mainKey.clear();
    IV.clear();
    for (int i=0; i<16; i++) {
        mainKey.push_back(KEY_ARR[i]);
        IV.push_back(IV_ARR[i]);
    }

    key_schedule(expanded_key, KEY_ARR);
}

bool isBalanced(const vector<byte> byte256)
{
    byte res = 0;
    for (int i=0; i<256; i++) res ^= byte256[i];
    return res==0;
}

void makeDeltaSet(vector<vector<byte> plaintexts, int pos) // active pos
{
    int i, j;
    static vector<int> rand256;
    if (rand256.size() != 256) {
        rand256.reserve(256);
        for (i=0; i<256; i++) rand256.push_back(i);
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


void recover4byteRound5(vector<byte> &res, int col)
{
    vector<byte> plaintexts[256], ciphertexts[256], states[256];
    set<uint> pb4;
    vector<uint> removed;
    vector<byte> removedByte5, byte256;
    byte guess[4];    // 4 bytes of 4 rows
    map<unsigned int, set<byte> > lastByteCandidates;

    int ntest = 10;
    vector<vector<byte>> cipherList, plaintList;
    cipherList.resize(ntest);
    plaintList.resize(ntest);
    for (int t=0; t<ntest; t++) {
        cipherList[i].
    }
    for (int i=0; i<256; i++) {
        plaintexts[i].resize(16);
        ciphertexts[i].resize(16);
        states[i].resize(16);
    }


    uint maxk = 16;
    uint v0 = maxk*maxk*maxk;
    uint v1 = maxk*maxk;
    uint v2 = maxk;
    for (uint k=0; k<maxk*maxk*maxk*maxk; k++) pb4.insert(k);

    removed.reserve(maxk*maxk*maxk*maxk);
    byte256.resize(256);

    while (true) {
        bool finished = true;
        if (pb4.size() > 1) finished = false;
        if (finished) break;
        cout << "remaining = " << pb4.size() << "\n";

        makeDeltaSet(plaintexts, rand() % 16);
        for (int i=0; i<256; i++) aesEncryptVector(ciphertexts[i], plaintexts[i], expanded_key, 5);

        //----------------
        bool foundres = 0;
        removed.clear();

        for (set<uint>::iterator it=pb4.begin(); it!=pb4.end(); it++) {
            uint k = (*it);
            guess[3] = k%maxk;
            guess[2] = (k/v2) % maxk;
            guess[1] = (k/v1) % maxk;
            guess[0] = (k/v0) % maxk;
            if (lastByteCandidates.find(k)==lastByteCandidates.end()) {
                for (int i=0; i<256; i++) lastByteCandidates[k].insert(i);
            }

            for (int i=0; i<256; i++) {
                byte byteRow[4];
                for (int row=0; row<4; row++) {
                    byteRow[row] = subByteValue(ciphertexts[i][cell(row, col-row)] ^ guess[row], DECRYPT);
                }

                byte256[i] = MULT_E(byteRow[0]) ^ MULT_B(byteRow[1]) ^ MULT_D(byteRow[2]) ^ MULT_9(byteRow[3]);
            }

            removedByte5.clear();
            for (set<byte>::iterator itb=lastByteCandidates[k].begin(); itb!=lastByteCandidates[k].end(); itb++) {
                byte guessByte = (*itb);
                byte xor256 = 0, balancedByte = 0;

                for (int i=0; i<256; i++) {
                    balancedByte = subByteValue(byte256[i] ^ guessByte, DECRYPT);
                    xor256 ^= balancedByte;
                }

                if (xor256!=0) {
                    removedByte5.push_back(guessByte);
                }
            }

            for (int i=0; i<removedByte5.size(); i++) lastByteCandidates[k].erase(removedByte5[i]);
            if (lastByteCandidates[k].size()==0) removed.push_back(k);
        }

        for (int i=0; i<removed.size(); i++) pb4.erase(removed[i]);
    }

    uint k = (*pb4.begin());
    res.clear();
    res.push_back((k/v0) % maxk);
    res.push_back((k/v1) % maxk);
    res.push_back((k/v2) % maxk);
    res.push_back(k% maxk);
}

bool kt = 0;
int main()
{
    srand(5);
    init(false);

    cout << "The 11 keys are: \n";
    for (int j=0; j<16; j++) cout << int(mainKey[j]) << " "; cout << "\n";
    for (int i=0; i<11; i++) {
        cout << i << ": "; for (int j=0; j<16;j++) cout << int(expanded_key[16*i + j]) << " ";
        cout << "\n";
    }
    cout << "\n\n";

    // ***********************************************
    cout << "/*********************************************/\n";

    cout << "\n\n";
    vector<byte> tmp;
    vector<byte> lastKey;
    lastKey.resize(16);


    for (int col=0; col<4; col++) {
        recover4byteRound5(tmp, col);
        for (int row=0; row<4; row++) lastKey[cell(row,col-row)] = tmp[row];
    }
    for (int i=0; i<16; i++) cout << int(lastKey[i]) << " ";

    int xx;
    scanf("%d\n", &xx);
    /*
    int col = 0;
    recover4byteRound5(tmp, col);
    for (int row=0; row<4; row++) {
        lastKey[cell(row,0-row)] = tmp[row];
        cout << int(lastKey[cell(row,0-row)]) << " " << int(expanded_key);
    }
    */
    return 0;

    cout << "round 5 key = ";
    for (int i=0; i<16; i++) cout << int(lastKey[i]) << " ";
    cout << "\n\n";

    vector<byte> secretKeys;    // store all nround+1 keys
    invertKeyExpansion(secretKeys, lastKey, 5);

    cout << "secret key = ";
    for (int i=0; i<16; i++) cout << int(secretKeys[i]) << " ";
    cout << "\n";

    return 0;
}

