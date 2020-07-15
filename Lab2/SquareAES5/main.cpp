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

byte KEY_ARR[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 15};
byte expanded_key[176];

const int MAX_KEY = 32;
vector<byte> mainKey;
vector<byte> expandedKey;

void chooseLastKey()
{
    //byte key5[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    byte key5[16] = {31, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 5};
    //byte key5[16] = {15, 17, 20, 31, 14, 15, 26, 27, 26, 15, 5, 7, 9, 10, 20, 5};
    vector<byte> lastKey;
    for (int i=0; i<16; i++) lastKey.push_back(key5[i]);
    invertKeyExpansion(mainKey, lastKey, 5);
    for (int i=0; i<16; i++) KEY_ARR[i] = mainKey[i];
}

void init()
{
    mainKey.clear();
    for (int i=0; i<16; i++) mainKey.push_back(KEY_ARR[i]);

    key_schedule(expanded_key, KEY_ARR);
    expandedKey.clear();
    for (int i=0; i<176; i++) expandedKey.push_back(expanded_key[i]);
}

void makeDeltaSet(vector<vector<byte>> &plaintexts, int pos) // active pos
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


long long op = 0;
void recover4byteRound5(byte res[4], int col)
{
    clock_t starter, ender, st, ed;
    byte guess[4];    // 4 bytes of 4 rows
    byte byte256[256];
    bool foundres = false;

    double time0, time1, time2, time3, time4, time5;

    // 20 delta-sets, each delta set has 256 plaintext, each plaintext = 1 state = 16 bytes
    const int ntest = 10;
    vector<vector<byte>> plaintList[ntest], cipherList[ntest];
    for (int t=0; t<ntest; t++) {
        plaintList[t].resize(256);
        cipherList[t].resize(256);

        for (int i=0; i<256; i++) {
            plaintList[t][i].resize(16);
            cipherList[t][i].resize(16);
        }
        makeDeltaSet(plaintList[t], rand() % 16);
        for (int i=0; i<256; i++) aesEncryptVector(cipherList[t][i], plaintList[t][i], expandedKey, 5);
    }


    for (int b0=0; b0<MAX_KEY; b0++) {
        double mean = 0; uint cnt = 0;
        time0 = 0; time1 = 0; time2 = 0;
        guess[0] = b0;

        st = clock();
        for (int b1=0; b1<MAX_KEY; b1++)
        for (int b2=0; b2<MAX_KEY; b2++)
        for (int b3=0; b3<MAX_KEY; b3++)
        {
            op++;
            guess[1] = b1;
            guess[2] = b2;
            guess[3] = b3;
            bool foundres = true;

            int nbCandidate = 256;
            bool invalidCandidate[256];
            memset(invalidCandidate, 0, sizeof(invalidCandidate));

            for (int t=0; t<ntest; t++) {
                byte byteRow[4];

                for (int i=0; i<256; i++) {
                    for (int row=0; row<4; row++)
                        byteRow[row] = subByteValue(cipherList[t][i][cell(row, col-row)] ^ guess[row], DECRYPT);
                    byte256[i] = MULT_E(byteRow[0]) ^ MULT_B(byteRow[1]) ^ MULT_D(byteRow[2]) ^ MULT_9(byteRow[3]);
                    op += 10;
                }

                // NOTE TO SELF: byte go from 0->255, so in the final loop, lastByte++ cause it to become 0 -> it's < 256 -> loop forever. Use int instead
                for (int lastByte=0; lastByte<256; lastByte++) {
                    if (invalidCandidate[lastByte]) continue;

                    byte xor256 = 0;
                    for (int i=0; i<256; i++) {
                        xor256 ^= subByteValue(byte256[i] ^ lastByte, DECRYPT);
                        op += 2;
                    }
                    if (xor256!=0) {invalidCandidate[lastByte] = true; nbCandidate--; mean += t; cnt++;}

                    op++;
                    if (nbCandidate==0) break;
                }

                if (nbCandidate==0) break;
            }

            // if a 4-byte guess can pass 10 delta-set, we assume it is correct.
            // Each delta-set reduce key space by 2^7 in the worst case. 10 delta-set -> can reduce 2^70 keys.
            // We search for 5 bytes = 2^40 keys, so 10 delta-set is more than enough to ensure safety
            if (nbCandidate>0) {
                for (int i=0; i<4; i++) res[i] = guess[i];
                return;
            }
        }


        ed = clock();
        time0 = double(ed-st) / CLOCKS_PER_SEC;

        cout << "Time0 = " << time0 << "\n";
       // cout << "Time1 = " << time1 << "\n";
        //cout << "Time2 = " << time2 << "\n";
        cout << "Mean failed at round: " << mean/cnt << "\n";
        cout << double(guess[0]+1)/MAX_KEY*100 << "% completed\n";
        cout << "\n";
    }

    cout << "Error: cannot find key\n";
}

bool kt = 0;
int main()
{
    srand(5);
    chooseLastKey();
    init();
    clock_t starter, ender;
    starter = clock();

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
    byte tmp[4];
    vector<byte> lastKey;
    lastKey.resize(16);


    for (int col=0; col<4; col++) {
        recover4byteRound5(tmp, col);
        for (int row=0; row<4; row++) lastKey[cell(row,col-row)] = tmp[row];
    }

    ender = clock();
    double totalTime = double(ender-starter) / CLOCKS_PER_SEC;
    cout << "total time = " << totalTime << "\n";
    cout << "Number of calculation: " << op << "\n";

    cout << "round 5 key = ";
    for (int i=0; i<16; i++) cout << int(lastKey[i]) << " ";
    cout << "\n\n";

    vector<byte> secretKeys;    // store all nround+1 keys
    invertKeyExpansion(secretKeys, lastKey, 5);

    cout << "secret key = ";
    for (int i=0; i<16; i++) cout << int(secretKeys[i]) << " ";
    cout << "\n";

    int xx;
    scanf("%d",&xx);
    return 0;

    return 0;
}

