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
const int nbText = 256;
typedef map<int,vector<byte> > MapVectorByte ;

byte KEY_ARR[16] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4C, 0x3C};
byte IV_ARR[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte expanded_key[176];

vector<byte> mainKey;
vector<byte> IV;
vector<byte> plaintexts[256];
vector<byte> ciphertexts[256];

void init(bool randoming)
{
    if (randoming) {
        for (int i=0; i<16; i++) KEY_ARR[i] = rand() % 256;
    }

    for (int i=0; i<16; i++) {
        mainKey.push_back(KEY_ARR[i]);
        IV.push_back(IV_ARR[i]);
    }
    key_schedule(expanded_key, KEY_ARR);
}

void makeState(vector<byte> &state)
{
    state.clear();
    for (int j=0; j<16; j++) state.push_back(rand() % 256); // each state has exactly 16 byte
}

bool isBalanced(const vector<byte> states[], int pos)
{
    byte res = 0;
    for (int i=0; i<256; i++) res ^= states[i][pos];
    return res==0;
}

bool isBalanced(const vector<byte> byte256)
{
    if (byte256.size() != 256) {
        cout << "isBalanced wrong vector size crashing\n";
    }
    byte res = 0;
    for (int i=0; i<256; i++) res ^= byte256[i];
    return res==0;
}

void makeDeltaSet(vector<byte> plaintexts[], int pos) // active pos
{
    vector<int> rand256;
    rand256.clear();
    for (int i=0; i<256; i++) rand256.push_back(i);
    random_shuffle(rand256.begin(), rand256.end());

    //
    for (int i=0; i<256; i++) makeState(plaintexts[i]);

    plaintexts[0][pos] = rand256[0];
    for (int i=1; i<256; i++) {
        plaintexts[i][pos] = rand256[i];
        for (int  j=0; j<16; j++)
            if (j!=pos) plaintexts[i][j] = plaintexts[i-1][j];
    }
}

void setup4encrypt(vector<byte> ciphertexts[], const vector<byte> plaintexts[])
{
    for (int i=0; i<256; i++) {
        makeState(ciphertexts[i]);
        aesEncryptVector(ciphertexts[i], plaintexts[i], mainKey, 4);
    }
}


void reverseStatesPos(vector<byte> &res, const vector<byte> states[], int pos, byte guessKey)
{
    if (res.size() != 256) {
        res.clear();
        for (int i=0; i<256; i++) res.push_back(0);
    }

    for (int i=0; i<256; i++) {
        res[i] = subByteValue(states[i][pos] ^ guessKey, DECRYPT);
    }
}

int main()
{
    srand(time(NULL));
    init(true);

    cout << "The 11 keys are: \n";
    for (int j=0; j<16; j++) cout << int(mainKey[j]) << " "; cout << "\n";
    for (int i=0; i<11; i++) {
        cout << i << ": "; for (int j=0; j<16;j++) cout << int(expanded_key[16*i + j]) << " ";
        cout << "\n";
    }
    cout << "\n\n";

    // ***********************************************
    vector<byte> byte256;
    for (int i=0; i<256;i++) byte256.push_back(0);

    set<byte> possibleK4bytes[16];
    for (int i=0; i<16; i++) {
        for (int j=0; j<256; j++) possibleK4bytes[i].insert(j); // at the beginning, each byte has 256 possible values
    }

    cout << "Each line is number of remaining candidate bytes at position i of the last key\n";
    while (true)
    {
        bool finished = true;
        for (int i=0; i<16; i++) {
            if (possibleK4bytes[i].size() > 1) finished = false;
            cout << possibleK4bytes[i].size() << " ";
        }
        cout << "\n";
        if (finished) break;

        //---
        makeDeltaSet(plaintexts, rand() % 16);
        setup4encrypt(ciphertexts, plaintexts);

        set<byte>::iterator it;
        for (int pos=0; pos<16; pos++) {
            if (possibleK4bytes[pos].size() == 1) continue; // already found the correct value of this byte

            vector<byte> falseGuess; // store incorrect guesses to remove them from the candidate pool
            falseGuess.clear();
            for (it=possibleK4bytes[pos].begin(); it!=possibleK4bytes[pos].end(); it++) {
                byte guessKey = (*it);
                reverseStatesPos(byte256, ciphertexts, pos, guessKey);
                if (!isBalanced(byte256)) falseGuess.push_back(guessKey);
            }

            for (int i=0; i<falseGuess.size(); i++) possibleK4bytes[pos].erase(falseGuess[i]);
        }
    }

    vector<byte> lastKey;
    for (int i=0; i<16; i++) lastKey.push_back(int(*possibleK4bytes[i].begin()));
    cout << "round 4 key = ";
    for (int i=0; i<16; i++) cout << int(lastKey[i]) << " ";
    cout << "\n\n";

    vector<byte> secretKeys;    // store all nround+1 keys
    invertKeyExpansion(secretKeys, lastKey, 4);

    cout << "secret key = ";
    for (int i=0; i<16; i++) cout << int(secretKeys[i]) << " ";
    cout << "\n";

    return 0;
}
