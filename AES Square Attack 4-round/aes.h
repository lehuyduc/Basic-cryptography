
#include <stdio.h>
#include <string.h>
#include <vector>
using namespace std;

#define ENCRYPT 1
#define DECRYPT 2

#define MULT_2(x) (  xtime(x)  )
#define MULT_3(x) (  xtime(x) ^ (x)  )
#define MULT_9(x) (  xtime(xtime(xtime(x))) ^ (x)  )
#define MULT_B(x) (  xtime(xtime(xtime(x)) ^ (x)) ^ (x)  )
#define MULT_D(x) (  xtime(xtime(xtime(x) ^ (x))) ^ (x)  )
#define MULT_E(x) (  xtime(xtime(xtime(x) ^ (x)) ^ (x))  )

typedef unsigned char byte;
typedef unsigned int uint;

byte sbox[256] = {99, 124, 119, 123, 242, 107, 111, 197, 48, 1, 103, 43, 254, 215, 171, 118, 202, 130, 201, 125, 250, 89, 71, 240, 173, 212, 162, 175, 156, 164, 114, 192, 183, 253, 147, 38, 54, 63, 247, 204, 52, 165, 229, 241, 113, 216, 49, 21, 4, 199, 35, 195, 24, 150, 5, 154, 7, 18, 128, 226, 235, 39, 178, 117, 9, 131, 44, 26, 27, 110, 90, 160, 82, 59, 214, 179, 41, 227, 47, 132, 83, 209, 0, 237, 32, 252, 177, 91, 106, 203, 190, 57, 74, 76, 88, 207, 208, 239, 170, 251, 67, 77, 51, 133, 69, 249, 2, 127, 80, 60, 159, 168, 81, 163, 64, 143, 146, 157, 56, 245, 188, 182, 218, 33, 16, 255, 243, 210, 205, 12, 19, 236, 95, 151, 68, 23, 196, 167, 126, 61, 100, 93, 25, 115, 96, 129, 79, 220, 34, 42, 144, 136, 70, 238, 184, 20, 222, 94, 11, 219, 224, 50, 58, 10, 73, 6, 36, 92, 194, 211, 172, 98, 145, 149, 228, 121, 231, 200, 55, 109, 141, 213, 78, 169, 108, 86, 244, 234, 101, 122, 174, 8, 186, 120, 37, 46, 28, 166, 180, 198, 232, 221, 116, 31, 75, 189, 139, 138, 112, 62, 181, 102, 72, 3, 246, 14, 97, 53, 87, 185, 134, 193, 29, 158, 225, 248, 152, 17, 105, 217, 142, 148, 155, 30, 135, 233, 206, 85, 40, 223, 140, 161, 137, 13, 191, 230, 66, 104, 65, 153, 45, 15, 176, 84, 187, 22};

byte inv_sbox[256] = {82, 9, 106, 213, 48, 54, 165, 56, 191, 64, 163, 158, 129, 243, 215, 251, 124, 227, 57, 130, 155, 47, 255, 135, 52, 142, 67, 68, 196, 222, 233, 203, 84, 123, 148, 50, 166, 194, 35, 61, 238, 76, 149, 11, 66, 250, 195, 78, 8, 46, 161, 102, 40, 217, 36, 178, 118, 91, 162, 73, 109, 139, 209, 37, 114, 248, 246, 100, 134, 104, 152, 22, 212, 164, 92, 204, 93, 101, 182, 146, 108, 112, 72, 80, 253, 237, 185, 218, 94, 21, 70, 87, 167, 141, 157, 132, 144, 216, 171, 0, 140, 188, 211, 10, 247, 228, 88, 5, 184, 179, 69, 6, 208, 44, 30, 143, 202, 63, 15, 2, 193, 175, 189, 3, 1, 19, 138, 107, 58, 145, 17, 65, 79, 103, 220, 234, 151, 242, 207, 206, 240, 180, 230, 115, 150, 172, 116, 34, 231, 173, 53, 133, 226, 249, 55, 232, 28, 117, 223, 110, 71, 241, 26, 113, 29, 41, 197, 137, 111, 183, 98, 14, 170, 24, 190, 27, 252, 86, 62, 75, 198, 210, 121, 32, 154, 219, 192, 254, 120, 205, 90, 244, 31, 221, 168, 51, 136, 7, 199, 49, 177, 18, 16, 89, 39, 128, 236, 95, 96, 81, 127, 169, 25, 181, 74, 13, 45, 229, 122, 159, 147, 201, 156, 239, 160, 224, 59, 77, 174, 42, 245, 176, 200, 235, 187, 60, 131, 83, 153, 97, 23, 43, 4, 126, 186, 119, 214, 38, 225, 105, 20, 99, 85, 33, 12, 125};

byte rcon[11] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

byte cxtime[256], cMULT_2[256], cMULT_3[256], cMULT_9[256], cMULT_B[256], cMULT_D[256], cMULT_E[256];

void key_schedule(byte* expanded_key, byte* key);
void add_round_key(byte* state, byte* round_key);
void sub_bytes(byte* state, unsigned int mode);
void shift_rows(byte* state, unsigned int mode);            // modified to increase speed
void mix_columns(byte* state, unsigned int mode);
byte xtime(byte);
void display_state(byte* state, char *label);

// names like rot_word are C code, names like rotWord are C++ code
void aes_encrypt_nround(byte* cipher, byte* message, byte* expanded_key, int nround);
void aes_decrypt_nround(byte* message, byte* cipher, byte* expanded_key, int nround);
void rot_word(byte* col, unsigned int mode);
void sub_word(byte* col, unsigned int mode);

int cell(int row, int col);
byte subByteValue(byte value, unsigned int mode);
void aesEncryptVector(vector<byte> &cipher, vector<byte> &message, vector<byte> expandedKey, int nround);
void aesDecryptVector(vector<byte> &message, vector<byte> &cipher, vector<byte> expandedKey, int nround);
void invertKeyExpansion(vector<byte> &allKeys, const vector<byte> &lastKey, int nround);

// precompute xtime(x), MULT_2(x), MULT_3(x), ... , MULT_E(x)
void initPrecomputeAES();

/*************************************************************************************************************/

void aesEncryptVector(vector<byte> &cipher, vector<byte> &message, vector<byte> expandedKey, int nround)
{
    aes_encrypt_nround(&cipher[0], &message[0], &expandedKey[0], nround);
}

void aesDecryptVector(vector<byte> &message, vector<byte> &cipher, vector<byte> expandedKey, int nround)
{
    aes_decrypt_nround(&message[0], &cipher[0], &expandedKey[0], nround);
}

/*************/

byte subByteValue(byte value, unsigned int mode)
{
    if (mode==ENCRYPT) return sbox[value];
    else return inv_sbox[value];
}

/*************/

void rot_word(byte* col, unsigned int mode) // col is byte[4]
{
    if (mode==ENCRYPT) {
        byte tmp = col[0];
        col[0] = col[1];
        col[1] = col[2];
        col[2] = col[3];
        col[3] = tmp;
    }
    else {
        byte tmp = col[3];
        col[3] = col[2];
        col[2] = col[1];
        col[1] = col[0];
        col[0] = tmp;
    }
}

void sub_word(byte* col, unsigned int mode) // col is byte[4]
{
    for (int i=0; i<4; i++) {
        if (mode==ENCRYPT) col[i] = sbox[col[i]];
        else               col[i] = inv_sbox[col[i]];
    }
}

const int colat[12] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
inline int cell(int row, int col)
{
    col = colat[col + 4];   // col = (col + 4) % col; we precompute this using an array because mod (%) is a very costly operation
    return col*4 + row;     // AES column-based structure
}

void mixColumns(vector<byte> &key, unsigned int mode)
{
    mix_columns(&key[0], mode);
}


void invertKeyExpansion(vector<byte> &allKeys, const vector<byte> &lastKey, int nround)
{
    byte expandedKey[176];
    memcpy(expandedKey + nround*16, &lastKey[0], 16);

    for (int round=nround-1; round>=0; round--) {
        byte firstCol[4], lastCol[4], nextFirstCol[4];

        // for column 1->3: key(i,j) = key(i,j-1) xor lastRoundKey(i,j)
        // -> if we go left to right (key expansion): expandedKey[pos] = expandedKey[pos-4] xor expandedKey[pos-16]
        // -> if we go right to left (reverse steps): expandedKey[pos-16] becomes expandedKey[pos], expandedKey[pos-4] becomes expandedKey[pos+12], expandedKey[pos] becomes expandedKey[pos+16]
        for (int col=3; col>=1; col--)
            for (int row=0; row<=3; row++) {
                int pos = round*16 + cell(row,col);                           // position current round key at cell (row,col)
                expandedKey[pos] = expandedKey[pos+16] ^ expandedKey[pos+12];
                if (col==3) lastCol[row] = expandedKey[pos];
            }

        // the first column is created using this way: https://www.davidwong.fr/blockbreakers/aes_4_key_scheduler.html
        rot_word(lastCol, ENCRYPT);  // step 1
        sub_word(lastCol, ENCRYPT);   // step 2

        memcpy(nextFirstCol, expandedKey + (round+1)*16, 4);    // first column of next round key
        nextFirstCol[0] ^= rcon[round+1];  // step 4

        for (int row=0; row<=3; row++) {
            int pos = round*16 + cell(row,0);
            expandedKey[pos] = lastCol[row] ^ nextFirstCol[row];    // step 3
        }
    }

    allKeys.clear();
    for (int i=0; i<(nround+1)*16; i++) allKeys.push_back(expandedKey[i]);
}

/*************/
void aes_encrypt_nround(byte* cipher, byte* message, byte* expanded_key,int nround)
{
    byte *p_round_key;
    byte state[176];

    unsigned int n0_round;

    memcpy(state, message, 16);

    p_round_key = expanded_key;
    add_round_key(state, p_round_key);

    for (n0_round = 1; n0_round < nround; n0_round++)
    {
        sub_bytes(state, ENCRYPT);

        shift_rows(state, ENCRYPT);

        mix_columns(state, ENCRYPT);

        p_round_key += 16;
        add_round_key(state, p_round_key);
    }

    // last round.
    // The original code has an if statement inside the above loop to check if n0_round==nround or not.
    // It causes branch prediction fail which reduce speed, so we put it out of the loop.
    sub_bytes(state, ENCRYPT);
    shift_rows(state, ENCRYPT);
    p_round_key += 16;
    add_round_key(state, p_round_key);

    memcpy(cipher, state, 16);
}

void aes_decrypt_nround(byte* message, byte* cipher, byte* expanded_key, int nround)
{
    byte *p_round_key;
    byte state[176];

    unsigned int n0_round;

    memcpy(state, cipher, 16);

    p_round_key = &(expanded_key[nround*16]); // nround = 10 -> expanded_key[160]
    add_round_key(state, p_round_key);

    for (n0_round = 1; n0_round < nround; n0_round++)
    {
        shift_rows(state, DECRYPT);

        sub_bytes(state, DECRYPT);

        p_round_key -= 16;
        add_round_key(state, p_round_key);

        mix_columns(state, DECRYPT);
    }

    shift_rows(state, DECRYPT);
    sub_bytes(state, DECRYPT);
    p_round_key -= 16;
    add_round_key(state, p_round_key);

    memcpy(message, state, 16);
}

/***************************************************************/

void key_schedule(byte* expanded_key, byte* key)
{
  unsigned int i;


  for (i = 0; i < 176; i++)
  {
    if (i < 16)
      expanded_key[i] = key[i];
    else
    {
      if ((i/4) % 4 == 0)
      {
        if ((i % 16) == 0)
          expanded_key[i] = sbox[expanded_key[4*(i/4-1)+((i+1)%4)]] ^ rcon[i/16] ^ expanded_key[i-16];
        else
          expanded_key[i] = sbox[expanded_key[4*(i/4-1)+((i+1)%4)]] ^ expanded_key[i-16];
      }
      else
      {
        expanded_key[i] = expanded_key[i-4] ^ expanded_key[i-16];
      }
    }
  }
}

/***************************************************************/

void add_round_key(byte* state, byte* round_key)
{
  unsigned int n0_byte;


  for (n0_byte = 0; n0_byte < 16; n0_byte++)
  {
    state[n0_byte] = state[n0_byte] ^ round_key[n0_byte];
  }
}

/***************************************************************/

void sub_bytes(byte* state, unsigned int mode)
{
  unsigned int n0_byte;


  if (mode == ENCRYPT)
  {
    for (n0_byte = 0; n0_byte < 16; n0_byte++)
    {
      state[n0_byte] = sbox[state[n0_byte]];
    }
  }
  else
  {
    for (n0_byte = 0; n0_byte < 16; n0_byte++)
    {
      state[n0_byte] = inv_sbox[state[n0_byte]];
    }
  }
}

/***************************************************************/

void shift_rows(byte* state, unsigned int mode)
{
    byte temp;


    if (mode == ENCRYPT)
    {
        // Row 1
        temp = state[1];
        state[1] = state[5];
        state[5] = state[9];
        state[9] = state[13];
        state[13] = temp;

        // Row 3
        temp = state[3];
        state[3] = state[15];
        state[15] = state[11];
        state[11] = state[7];
        state[7] = temp;
    }
    else
    {
        temp = state[1];
        state[1] = state[13];
        state[13] = state[9];
        state[9] = state[5];
        state[5] = temp;

        temp = state[3];
        state[3] = state[7];
        state[7] = state[11];
        state[11] = state[15];
        state[15] = temp;
    }


    // Row 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;
}

/***************************************************************/

void mix_columns(byte* state, unsigned int mode)
{
  byte *p_state;
  byte out[4];

  unsigned int i, j;

    // I replaced all MULT_2(x), MULT_3(x), ... macros by precomputed array. This greatly increase speed.
    // For example,  MULT_E(x) == (  xtime(xtime(xtime(x) ^ (x)) ^ (x))  ) consist of multiple if statements, calculations, mem access.
    // cMULT_E[x] cost only 1 memory access.

  if (mode == ENCRYPT)
  {
    p_state = state;
    for (j = 0; j < 4; j++)
    {
      for (i = 0; i < 4; i++)
      {
        out[i] = cMULT_2[p_state[(0 + i) % 4]] ^ cMULT_3[p_state[(1 + i) % 4]] ^ p_state[(2 + i) % 4] ^ p_state[(3 + i) % 4];
      }
      memcpy(p_state, out, 4);
      p_state += 4;
    }
  }
  else
  {
    p_state = state;
    for (j = 0; j < 4; j++)
    {
      for (i = 0; i < 4; i++)
      {
        out[i] = cMULT_E[p_state[(0 + i) % 4]] ^ cMULT_B[p_state[(1 + i) % 4]] ^ cMULT_D[p_state[(2 + i) % 4]] ^ cMULT_9[p_state[(3 + i) % 4]];
      }
      memcpy(p_state, out, 4);
      p_state += 4;
    }
  }
}

/***************************************************************/

byte xtime(byte byte)
{
  if ((byte & 0x80) == 0)
  {
    return (byte << 1);
  }
  else
  {
    return (byte << 1) ^ 0x1B;
  }
}

void initPrecomputeAES()
{
    for (int i=0; i<256; i++) {
        byte x = i;
        cxtime[x] = xtime(x);
        cMULT_2[x] = xtime(x);
        cMULT_3[x] = xtime(x) ^ (x);
        cMULT_9[x] = xtime(xtime(xtime(x))) ^ (x);
        cMULT_B[x] = xtime(xtime(xtime(x)) ^ (x)) ^ (x);
        cMULT_D[x] = xtime(xtime(xtime(x) ^ (x))) ^ (x);
        cMULT_E[x] = xtime(xtime(xtime(x) ^ (x)) ^ (x));
    }
}

/***************************************************************/

void display_state(byte* state, char *label)
{
  int i;

  printf("%s\n", label);
  for (i = 0; i < 4; i++)
  {
    printf("%02X %02X %02X %02X\n", state[i + 0], state[i + 4], state[i + 8], state[i + 12]);
  }
  printf("\n");
}


