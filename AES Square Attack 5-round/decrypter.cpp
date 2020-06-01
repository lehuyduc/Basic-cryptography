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

int MAX_KEY = 255;
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

void recover4byteRound5(byte res[4], int col, byte lastRowByte)
{
    clock_t starter, ender, st, ed;
    byte guess[4];    // 4 bytes of 4 rows of column "col"
    byte byte256[256];

    for (int b0=0; b0<=MAX_KEY; b0++) {
        guess[0] = b0;

        for (int b1=0; b1<=MAX_KEY; b1++)
        for (int b2=0; b2<=MAX_KEY; b2++)
        {
            guess[1] = b1;
            guess[2] = b2;
            guess[3] = lastRowByte;

            // guessing the byte of round 4 key
            int nbCandidate = 256;
            bool invalidCandidate[256];
            memset(invalidCandidate, 0, sizeof(invalidCandidate));

            // First step is converting the output state to the state at round 4 after MixColumnInv.
            // Second step is guess the round-4 key, and convert the delta-set back to the beginning of round 4.
            // We loop over test first instead of guessKeyByte first because the first one or two tests will kill most, and sometimes all candidate keys
            // this means we don't have to perform step 1 for all test sets.
            for (int t=0; t<ntest; t++) {
                byte byteRow[4];
                // we perform MixColumn before AddRoundKey
                // after this step, byte256 contains the targeted bytes in round 4 after MixColumnInv, before AddRoundKey and ShiftRowInv and SubByteInv
                for (int i=0; i<256; i++) {
                    for (int row=0; row<4; row++)
                        byteRow[row] = xorSubDecrypt[cipherList[t][i][cell(row, col-row)]][guess[row]];             //byteRow[row] = subByteValue(cipherList[t][i][cell(row, col-row)] ^ guess[row], DECRYPT);
                    byte256[i] = cMULT_E[byteRow[0]] ^ cMULT_B[byteRow[1]] ^ cMULT_D[byteRow[2]] ^ cMULT_9[byteRow[3]]; // MixColumn Decrypt the value on the first row only
                }

                // NOTE TO SELF: byte go from 0->255, so in the final loop, lastByte++ == 256%256 == 0 < 256 -> loop forever. Use int instead
                for (int lastByte=0; lastByte<256; lastByte++) {
                    if (invalidCandidate[lastByte]) continue;

                    // here is the deepest loop. Most of the time is spent here.
                    // This loop check if the byte at one position is balanced across 256 plaintexts
                    byte xor256 = 0;                                                            // byte256[i] ^ lastByte gives the byte value at the end of round 3
                    for (int i=0; i<256; i++) xor256 ^= xorSubDecrypt[byte256[i]][lastByte];    //speedup 30% compared to xor256 ^= subByteValue(byte256[i] ^ lastByte, DECRYPT);

                    if (xor256!=0) {
                        invalidCandidate[lastByte] = true;
                        nbCandidate--;
                    }

                    if (nbCandidate==0)  break;
                }

                if (nbCandidate==0) break;
            }

            // if a 4-byte guess can pass 10 delta-sets, we assume it is correct.
            // Each delta-set reduce key space by 2^7 in the worst case. 10 delta-set -> can reduce 2^70 keys.
            // We search for 5 bytes = 2^40 keys, so 10 delta-set is more than enough to ensure safety
            if (nbCandidate>0) {
                for (int i=0; i<4; i++) res[i] = guess[i];
                cout << "Find key for column " << col << " successful\n\n********************************\n";
                return;
            }
        }

        cout << setprecision(5) << double(guess[0]+1)/MAX_KEY*100 << "% completed\n";
        cout << "\n";
    }

    cout << "Error: cannot find key\n\n";
    exit(-1);
}

int main()
{
    initPrecomputeAES();
    initFastXorSub();

    int k;
    ifstream fin("encrypted.txt");
    fin >> ntest;
    fin >> MAX_KEY;
    for (int col=0; col<4; col++) {
        fin >> k;
        row4key[col] = k;
    }

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
    byte tmp[4];
    vector<byte> lastKey;
    lastKey.resize(16);

    for (int col=0; col<4; col++) {
        recover4byteRound5(tmp, col, row4key[(col+1)%4]);
        for (int row=0; row<4; row++) lastKey[cell(row,col-row)] = tmp[row];
    }

    cout << "*****************************\n";

    cout << "round 5 key = ";
    for (int i=0; i<16; i++) cout << int(lastKey[i]) << " ";
    cout << "\n\n";

    vector<byte> secretKeys;
    invertKeyExpansion(secretKeys, lastKey, 5);

    cout << "secret key = ";
    for (int i=0; i<16; i++) cout << int(secretKeys[i]) << " ";
    cout << "\n";


    //--------------------  WRITE RESULTS TO FILE
    ofstream fo("key5guess.txt");
    for (int i=0; i<16; i++) fo << int(lastKey[i]) << " ";
    fo << "\n";

    cout << "Finished decrypting successfully\n";
    cout << "*****************************\n";
    return 0;
}


