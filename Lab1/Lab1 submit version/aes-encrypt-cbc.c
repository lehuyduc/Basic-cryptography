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

int main(int argc, char* argv[])
{
    byte ciphertext_block[16];
    byte key[16] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    byte IV[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    byte plaintext_block[16];

    char name_ciphertext[100];
    char name_plaintext[100];

    FILE* pfd_ciphertext = NULL;
    FILE* pfd_plaintext = NULL;

    unsigned int nb_byte;


    if (argc != 3)
    {
    printf("Usage: %s plaintext_file ciphertext_file\n", argv[0]);
    exit(-1);
    }

    sprintf(name_plaintext, "%s", argv[1]);
    sprintf(name_ciphertext, "%s", argv[2]);

    printf("name_plaintext : %s\n", name_plaintext);
    printf("name_ciphertext : %s\n", name_ciphertext);

    if ((pfd_plaintext = fopen(name_plaintext, "rb")) == NULL)
    {
        printf("ERROR: can not open file \"%s\" for reading\n", name_plaintext);
        exit(-1);
    }

    if ((pfd_ciphertext = fopen(name_ciphertext, "wb")) == NULL)
    {
        printf("ERROR: can not open file \"%s\" for writing\n", name_ciphertext);
        exit(-1);
    }

    // complete here
    byte xored[16], prev_encrypt[16];
    memcpy(prev_encrypt, IV, 16);

    while ((nb_byte = fread(plaintext_block, sizeof(char), 16, pfd_plaintext)) == 16)
    {
        xor2array(xored, plaintext_block, prev_encrypt, 16);
        aes_encrypt(ciphertext_block, xored, key);
        fwrite(ciphertext_block, sizeof(char), 16, pfd_ciphertext);

        memcpy(prev_encrypt, ciphertext_block, 16);
    }

    //
    memset(plaintext_block + nb_byte, 16 - nb_byte, 16 - nb_byte);

    xor2array(xored, plaintext_block, prev_encrypt, 16);
    aes_encrypt(ciphertext_block, xored, key);
    fwrite(ciphertext_block, sizeof(char), 16, pfd_ciphertext);

    //
    fclose(pfd_plaintext);
    fclose(pfd_ciphertext);

  return 0;
}
