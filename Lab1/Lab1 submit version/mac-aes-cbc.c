#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "aes.h"

typedef unsigned char byte;

extern void aes_encrypt(byte* cipher, byte* message, byte* key);
extern void aes_decrypt(byte* message, byte* cipher, byte* key);

void display(byte* state, char *label);

void xor2array(char* res, const char* a1, const char* a2, int n)
{
    for (int i=0; i<n; i++) res[i] = a1[i] ^ a2[i];
}

void byte2hex(char* res, byte value)
{
    char hex[16];
    memcpy(hex, "0123456789ABCDEF", 16);
    res[1] = hex[value % 16];
    res[0] = hex[value / 16];
}

void array2hex(char *res, byte *arr)
{
    char str[32], tmp[2];

    for (int i=0; i<16; i++)
    {
        int value = arr[i];
        byte2hex(tmp, value);
        str[2*i] = tmp[0];
        str[2*i+1] = tmp[1];
    }

    memcpy(res, str, 32);
}

int main(int argc, char* argv[])
{
    byte ciphertext_block[16];
    byte key[16] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    byte IV[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    byte plaintext_block[16];

    char name_plaintext[100];
    FILE* pfd_plaintext = NULL;
    unsigned int nb_byte;


    if (argc != 2)
    {
        printf("Usage: %s plaintext_file\n", argv[0]);
        exit(-1);
    }

    sprintf(name_plaintext, "%s", argv[1]);

    printf("name_plaintext : %s\n", name_plaintext);

    if ((pfd_plaintext = fopen(name_plaintext, "rb")) == NULL)
    {
        printf("ERROR: can not open file \"%s\" for reading\n", name_plaintext);
        exit(-1);
    }

    // complete here
    byte xored[16], prev_encrypt[16];
    memcpy(prev_encrypt, IV, 16);

    while ((nb_byte = fread(plaintext_block, sizeof(char), 16, pfd_plaintext)) == 16)
    {
        xor2array(xored, plaintext_block, prev_encrypt, 16);
        aes_encrypt(ciphertext_block, xored, key);
        memcpy(prev_encrypt, ciphertext_block, 16);
    }

    //
    memset(plaintext_block + nb_byte, 16 - nb_byte, 16 - nb_byte);

    xor2array(xored, plaintext_block, prev_encrypt, 16);
    aes_encrypt(ciphertext_block, xored, key);

    //
    fclose(pfd_plaintext);

    char res[100];

    array2hex(res, ciphertext_block);
    res[32] = 0;
    printf("%s\n",res);

    return 0;
}

