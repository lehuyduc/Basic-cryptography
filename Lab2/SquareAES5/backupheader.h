
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

typedef unsigned char BYTE;
typedef BYTE byte;

extern bool kt;

BYTE sbox[256] = {99, 124, 119, 123, 242, 107, 111, 197, 48, 1, 103, 43, 254, 215, 171, 118, 202, 130, 201, 125, 250, 89, 71, 240, 173, 212, 162, 175, 156, 164, 114, 192, 183, 253, 147, 38, 54, 63, 247, 204, 52, 165, 229, 241, 113, 216, 49, 21, 4, 199, 35, 195, 24, 150, 5, 154, 7, 18, 128, 226, 235, 39, 178, 117, 9, 131, 44, 26, 27, 110, 90, 160, 82, 59, 214, 179, 41, 227, 47, 132, 83, 209, 0, 237, 32, 252, 177, 91, 106, 203, 190, 57, 74, 76, 88, 207, 208, 239, 170, 251, 67, 77, 51, 133, 69, 249, 2, 127, 80, 60, 159, 168, 81, 163, 64, 143, 146, 157, 56, 245, 188, 182, 218, 33, 16, 255, 243, 210, 205, 12, 19, 236, 95, 151, 68, 23, 196, 167, 126, 61, 100, 93, 25, 115, 96, 129, 79, 220, 34, 42, 144, 136, 70, 238, 184, 20, 222, 94, 11, 219, 224, 50, 58, 10, 73, 6, 36, 92, 194, 211, 172, 98, 145, 149, 228, 121, 231, 200, 55, 109, 141, 213, 78, 169, 108, 86, 244, 234, 101, 122, 174, 8, 186, 120, 37, 46, 28, 166, 180, 198, 232, 221, 116, 31, 75, 189, 139, 138, 112, 62, 181, 102, 72, 3, 246, 14, 97, 53, 87, 185, 134, 193, 29, 158, 225, 248, 152, 17, 105, 217, 142, 148, 155, 30, 135, 233, 206, 85, 40, 223, 140, 161, 137, 13, 191, 230, 66, 104, 65, 153, 45, 15, 176, 84, 187, 22};

BYTE inv_sbox[256] = {82, 9, 106, 213, 48, 54, 165, 56, 191, 64, 163, 158, 129, 243, 215, 251, 124, 227, 57, 130, 155, 47, 255, 135, 52, 142, 67, 68, 196, 222, 233, 203, 84, 123, 148, 50, 166, 194, 35, 61, 238, 76, 149, 11, 66, 250, 195, 78, 8, 46, 161, 102, 40, 217, 36, 178, 118, 91, 162, 73, 109, 139, 209, 37, 114, 248, 246, 100, 134, 104, 152, 22, 212, 164, 92, 204, 93, 101, 182, 146, 108, 112, 72, 80, 253, 237, 185, 218, 94, 21, 70, 87, 167, 141, 157, 132, 144, 216, 171, 0, 140, 188, 211, 10, 247, 228, 88, 5, 184, 179, 69, 6, 208, 44, 30, 143, 202, 63, 15, 2, 193, 175, 189, 3, 1, 19, 138, 107, 58, 145, 17, 65, 79, 103, 220, 234, 151, 242, 207, 206, 240, 180, 230, 115, 150, 172, 116, 34, 231, 173, 53, 133, 226, 249, 55, 232, 28, 117, 223, 110, 71, 241, 26, 113, 29, 41, 197, 137, 111, 183, 98, 14, 170, 24, 190, 27, 252, 86, 62, 75, 198, 210, 121, 32, 154, 219, 192, 254, 120, 205, 90, 244, 31, 221, 168, 51, 136, 7, 199, 49, 177, 18, 16, 89, 39, 128, 236, 95, 96, 81, 127, 169, 25, 181, 74, 13, 45, 229, 122, 159, 147, 201, 156, 239, 160, 224, 59, 77, 174, 42, 245, 176, 200, 235, 187, 60, 131, 83, 153, 97, 23, 43, 4, 126, 186, 119, 214, 38, 225, 105, 20, 99, 85, 33, 12, 125};

BYTE rcon[11] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

void aes_encrypt(BYTE* cipher, BYTE* message, BYTE* key);
void aes_decrypt(BYTE* message, BYTE* cipher, BYTE* key);
void key_schedule(BYTE* expanded_key, BYTE* key);
void add_round_key(BYTE* state, BYTE* round_key);
void sub_bytes(BYTE* state, unsigned int mode);
void shift_rows(BYTE* state, unsigned int mode);
void mix_columns(BYTE* state, unsigned int mode);
BYTE xtime(BYTE);
void display_state(BYTE* state, char *label);

// names like rot_word are C code, names like rotWord are C++ code
int cell(int row, int col);
void rot_word(BYTE* col, unsigned int mode);
void sub_word(BYTE* col, unsigned int mode);
byte subByteValue(const byte &value, const unsigned int &mode);
void subByteVector(vector<byte> &state, const unsigned int &mode);
void aes_encrypt_nround(BYTE* cipher, BYTE* message, BYTE* key,int nround);
void aes_decrypt_nround(BYTE* message, BYTE* cipher, BYTE* key, int nround);
void aesEncryptVector(vector<byte> &cipher, vector<byte> &message, vector<byte> &key, int nround);
void aesDecryptVector(vector<byte> &message, vector<byte> &cipher, vector<byte> &key, int nround);
void invertKeyExpansion(vector<byte> key, int nround);


/*************************************************************************************************************/

void aesEncryptVector(vector<byte> &cipher, vector<byte> &message, vector<byte> &key, int nround)
{
    aes_encrypt_nround(&cipher[0], &message[0], &key[0], nround);
}

void aesDecryptVector(vector<byte> &message, vector<byte> &cipher, vector<byte> &key, int nround)
{
    aes_decrypt_nround(&message[0], &cipher[0], &key[0], nround);
}

/*************/

void subByteVector(vector<byte> &state, const unsigned int &mode)
{
    sub_bytes(&state[0], mode);
}

byte subByteValue(const byte &value, const unsigned int &mode)
{
    if (mode==ENCRYPT) return sbox[value];
    else return inv_sbox[value];
}

/*************/

void rot_word(BYTE* col, unsigned int mode) // col is byte[4]
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

void sub_word(BYTE* col, unsigned int mode) // col is byte[4]
{
    for (int i=0; i<4; i++) {
        if (mode==ENCRYPT) col[i] = sbox[col[i]];
        else               col[i] = inv_sbox[col[i]];
    }
}

const int colat[12] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
inline int cell(int row,int col)
{
    col = colat[col + 4];
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

        for (int col=3; col>=1; col--)
            for (int row=0; row<=3; row++) {
                int pos = round*16 + cell(row,col);
                expandedKey[pos] = expandedKey[pos+16] ^ expandedKey[pos+12];
                if (col==3) lastCol[row] = expandedKey[pos];
            }

        rot_word(lastCol, ENCRYPT);  // step 1
        sub_word(lastCol, ENCRYPT);   // step 2

        memcpy(nextFirstCol, expandedKey + (round+1)*16, 4);
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
void aes_encrypt_nround(BYTE* cipher, BYTE* message, BYTE* key,int nround)
{
    BYTE expanded_key[176];
    BYTE *p_round_key;
    BYTE state[176];

    unsigned int n0_round;


    memcpy(state, message, 16);

    key_schedule(expanded_key, key);

    p_round_key = expanded_key;
    add_round_key(state, p_round_key);

    for (n0_round = 1; n0_round <= nround; n0_round++)
    {
        sub_bytes(state, ENCRYPT);

        shift_rows(state, ENCRYPT);

        if (n0_round < nround || kt)    // kt is a global var for debugging
        {
            mix_columns(state, ENCRYPT);
        }

    p_round_key += 16;
    add_round_key(state, p_round_key);
    }

    memcpy(cipher, state, 16);
}

void aes_decrypt_nround(BYTE* message, BYTE* cipher, BYTE* key, int nround)
{
  BYTE expanded_key[176];
  BYTE *p_round_key;
  BYTE state[176];

  unsigned int n0_round;

  memcpy(state, cipher, 16);

  key_schedule(expanded_key, key);

  p_round_key = &(expanded_key[nround*16]); // nround = 10 -> 160
  add_round_key(state, p_round_key);

  for (n0_round = 1; n0_round <= nround; n0_round++)
  {
    shift_rows(state, DECRYPT);

    sub_bytes(state, DECRYPT);

    p_round_key -= 16;
    add_round_key(state, p_round_key);

    if (n0_round < nround)
    {
      mix_columns(state, DECRYPT);
    }
  }

  memcpy(message, state, 16);
}

// ---------------------    END OF MY EXTRA  CODE.
// ---------------------    BELOW IS TEACHER CODE

/***************************************************************/

void aes_encrypt(BYTE* cipher, BYTE* message, BYTE* key)
{
    BYTE expanded_key[176];
    BYTE *p_round_key;
    BYTE state[176];

    unsigned int n0_round;


    memcpy(state, message, 16);

    key_schedule(expanded_key, key);

    p_round_key = expanded_key;
    add_round_key(state, p_round_key);

    for (n0_round = 1; n0_round <= 10; n0_round++)
    {
    sub_bytes(state, ENCRYPT);

    shift_rows(state, ENCRYPT);

    if (n0_round < 10)
    {
      mix_columns(state, ENCRYPT);
    }

    p_round_key += 16;
    add_round_key(state, p_round_key);
    }

    memcpy(cipher, state, 16);
}

/***************************************************************/

void aes_decrypt(BYTE* message, BYTE* cipher, BYTE* key)
{
  BYTE expanded_key[176];
  BYTE *p_round_key;
  BYTE state[176];

  unsigned int n0_round;


  memcpy(state, cipher, 16);

  key_schedule(expanded_key, key);

  p_round_key = &(expanded_key[160]);
  add_round_key(state, p_round_key);

  for (n0_round = 1; n0_round <= 10; n0_round++)
  {
    shift_rows(state, DECRYPT);

    sub_bytes(state, DECRYPT);

    p_round_key -= 16;
    add_round_key(state, p_round_key);

    if (n0_round < 10)
    {
      mix_columns(state, DECRYPT);
    }
  }

  memcpy(message, state, 16);
}

/***************************************************************/

void key_schedule(BYTE* expanded_key, BYTE* key)
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

void add_round_key(BYTE* state, BYTE* round_key)
{
  unsigned int n0_byte;


  for (n0_byte = 0; n0_byte < 16; n0_byte++)
  {
    state[n0_byte] = state[n0_byte] ^ round_key[n0_byte];
  }
}

/***************************************************************/

void sub_bytes(BYTE* state, unsigned int mode)
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

void shift_rows(BYTE* state, unsigned int mode)
{
  BYTE temp;


  // Row 1
  if (mode == ENCRYPT)
  {
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;
  }
  else
  {
    temp = state[1];
    state[1] = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = temp;
  }


  // Row 2
  temp = state[2];
  state[2] = state[10];
  state[10] = temp;
  temp = state[6];
  state[6] = state[14];
  state[14] = temp;

  // Row 3
  if (mode == ENCRYPT)
  {
    temp = state[3];
    state[3] = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = temp;
  }
  else
  {
    temp = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = temp;
  }
}

/***************************************************************/

void mix_columns(BYTE* state, unsigned int mode)
{
  BYTE *p_state;
  BYTE out[4];

  unsigned int i, j;


  if (mode == ENCRYPT)
  {
    p_state = state;
    for (j = 0; j < 4; j++)
    {
      for (i = 0; i < 4; i++)
      {
        out[i] = MULT_2(p_state[(0 + i) % 4]) ^ MULT_3(p_state[(1 + i) % 4]) ^ p_state[(2 + i) % 4] ^ p_state[(3 + i) % 4];
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
        out[i] = MULT_E(p_state[(0 + i) % 4]) ^ MULT_B(p_state[(1 + i) % 4]) ^ MULT_D(p_state[(2 + i) % 4]) ^ MULT_9(p_state[(3 + i) % 4]);
      }
      memcpy(p_state, out, 4);
      p_state += 4;
    }
  }
}

/***************************************************************/

BYTE xtime(BYTE byte)
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

/***************************************************************/

void display_state(BYTE* state, char *label)
{
  int i;

  printf("%s\n", label);
  for (i = 0; i < 4; i++)
  {
    printf("%02X %02X %02X %02X\n", state[i + 0], state[i + 4], state[i + 8], state[i + 12]);
  }
  printf("\n");
}



