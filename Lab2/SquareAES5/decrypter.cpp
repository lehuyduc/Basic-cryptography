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

byte KEY_ARR[16];
byte expanded_key[176];

int MAX_KEY = 256;
vector<byte> mainKey;
vector<byte> expandedKey;

int ntest;
byte row4key[4];
vector<vector<byte>> cipherList[22];

byte xorSubDecrypt[256][256];
/*************************************/

void initFastXorSub()
{
    for (uint a=0; a<256; a++)
    for (uint b=0; b<256; b++)
    {
        xorSubDecrypt[a][b] = subByteValue(a ^ b, DECRYPT);
    }
}

/*************************************/

void recover4byteRound5(byte res[4], int col, byte byteRow4)
{
    clock_t starter, ender, st, ed;
    byte guess[4];    // 4 bytes of 4 rows
    byte byte256[256];

    double time0, time1, time2, time3, time4, time5;

    for (int b0=0; b0<MAX_KEY; b0++) {
        guess[0] = b0;
        time1 = 0;
        time2 = 0;

        st = clock();
        for (int b1=0; b1<MAX_KEY; b1++)
        for (int b2=0; b2<MAX_KEY; b2++)
        {
            guess[1] = b1;
            guess[2] = b2;
            guess[3] = byteRow4;
            //cout << b2 << "\n";

            int nbCandidate = 256;
            bool invalidCandidate[256];
            memset(invalidCandidate, 0, sizeof(invalidCandidate));

            for (int t=0; t<ntest; t++) {
                byte byteRow[4];

                // after this step, byte256 contains the targeted bytes after MixColumnInv
                starter = clock();
                for (int i=0; i<256; i++) {
                    for (int row=0; row<4; row++)
                        byteRow[row] = xorSubDecrypt[cipherList[t][i][cell(row, col-row)]][guess[row]]; //byteRow[row] = subByteValue(cipherList[t][i][cell(row, col-row)] ^ guess[row], DECRYPT);
                    byte256[i] = MULT_E(byteRow[0]) ^ MULT_B(byteRow[1]) ^ MULT_D(byteRow[2]) ^ MULT_9(byteRow[3]);
                }
                ender = clock();
                time1 += double(ender-starter) / CLOCKS_PER_SEC;

                // NOTE TO SELF: byte go from 0->255, so in the final loop, lastByte++ == 256%256 == 0 < 256 -> loop forever. Use int instead
                for (int lastByte=0; lastByte<256; lastByte++) {
                    if (invalidCandidate[lastByte]) continue;

                    // here is the deepest loop. Most of the time is spent here.
                    // This loop check if the byte at one position is balanced across 256 plaintexts
                    starter = clock();
                    byte xor256 = 0;
                    for (int i=0; i<256; i++) {
                        xor256 ^= xorSubDecrypt[byte256[i]][lastByte]; //speedup 30% compared to xor256 ^= subByteValue(byte256[i] ^ lastByte, DECRYPT);
                    }
                    if (xor256!=0) {
                        invalidCandidate[lastByte] = true; nbCandidate--;
                    }
                    ender = clock();
                    time2 +=  double(ender-starter) / CLOCKS_PER_SEC;

                    if (nbCandidate==0) {t = ntest; break;}
                }

                //if (nbCandidate==0) break;
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

        cout << "Time per guess[0]        = " << time0 << "\n";
        cout << "Time to get mixColInv    = " << time1 << "\n";
        cout << "Time inside deepest loop = " << time2 << "\n";
        cout << double(guess[0]+1)/MAX_KEY*100 << "% completed\n";
        cout << "\n";
    }

    cout << "Error: cannot find key\n";
}

bool kt = 0;
int main()
{
    initFastXorSub();

    freopen("encrypted.txt", "r", stdin);
    scanf("%d", &ntest);
    scanf("%d", &MAX_KEY);
    for (int col=0; col<4; col++) scanf("%d", &row4key[col]);

    for (int t=0; t<ntest; t++) {
        cipherList[t].resize(256);
        for (int i=0; i<256; i++) {
            cipherList[t][i].resize(16);
            for (int j=0; j<16; j++) scanf("%d", &cipherList[t][i][j]);
        }
    }
    fclose(stdin);

    /****************************/
    byte tmp[4];
    vector<byte> lastKey;
    lastKey.resize(16);

    for (int col=0; col<4; col++) {
        recover4byteRound5(tmp, col, row4key[(col+1)%4]);
        for (int row=0; row<4; row++) lastKey[cell(row,col-row)] = tmp[row];
    }

    cout << "round 5 key = ";
    for (int i=0; i<16; i++) cout << int(lastKey[i]) << " ";
    cout << "\n\n";

    vector<byte> secretKeys;
    invertKeyExpansion(secretKeys, lastKey, 5);

    cout << "secret key = ";
    for (int i=0; i<16; i++) cout << int(secretKeys[i]) << " ";
    cout << "\n";

    printf("\nPress enter to exit\n");
    getchar();
    return 0;
}


